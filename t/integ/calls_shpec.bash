ROOT_DIR="$(dirname "$(readlink -f $BASH_SOURCE)")"

source "$ROOT_DIR/shpec_util.bash"

SERVER_ADDR='localhost:9000'

get_header () {
  sed -n "s/$1: //p" | tr -d '[:space:]'
}

get_status () {
  grep -Eo '\b[0-9]{3}\b'
}

export LD_LIBRARY_PATH=.

./integ >/dev/null &

trap "echo cleaning up...; kill $!; rm integ" EXIT

describe 'libhttp integration tests'
  it 'GET request returns payload'
    actual="$(curl "$SERVER_ADDR")"
    assert equal 'Hello World!' "$actual"
  ti

  it 'GET request returns 200 status'
    actual="$(curl -s -w "%{http_code}" "$SERVER_ADDR" -o /dev/null)"
    assert equal '200' "$actual"
  ti

  it 'sets the Content-Type header'
    actual="$(curl -i -s "$SERVER_ADDR" | get_header 'Content-Type')"
    assert equal 'text/plain' "$actual"
  ti

  it 'sets the X-powered-by header'
    actual="$(curl -i -s "$SERVER_ADDR" | get_header 'X-Powered-By')"
    assert equal 'integ-test' "$actual"
  ti

  it 'returns a 405 for unsupported methods'
    declare -a invalid_methods=('POST' 'PUT' 'PATCH' 'DELETE' 'HEAD' 'TRACE' 'CONNECT')

    for method in "${invalid_methods[@]}"; do
      actual="$(curl -X "$method" -s -w "%{http_code}" "$SERVER_ADDR")"
      assert equal '405' "$actual"
    done
  ti

  it 'returns 404 for invalid routes'
    actual="$(curl -s -w "%{http_code}" "$SERVER_ADDR/invalid")"
    assert equal '404' "$actual"
  ti

  it 'handles a request with duplicate headers'
    actual="$(curl -s -w "%{http_code}" -o /dev/null "$SERVER_ADDR" -H 'header:v' -H 'header:v2')"
    assert equal '200' "$actual"
  ti
end_describe

describe 'libhttp integration tests'
  ALLOWED_ORIGIN='test.com'
  ALLOWED_METHOD='DELETE'
  ALLOWED_HEADER='X-Test-Header'

  it 'rejects preflight requests with disallowed headers'
    actual="$(curl "$SERVER_ADDR" -s -i -H "Access-Control-Request-Method: $ALLOWED_METHOD" -H 'Access-Control-Request-Headers: X-Requested-With' -H "Origin: $ALLOWED_ORIGIN" -X OPTIONS)"

    vary_header=$(get_header 'Vary' <<< "$actual")
    assert equal "$vary_header" 'Origin,Access-Control-Request-Method,Access-Control-Request-Headers'

    status=$(get_status <<< "$actual")
    assert equal "$status" 204
  ti

  it 'rejects preflight requests with disallowed origins'
    actual="$(curl "$SERVER_ADDR" -s -i -H "Access-Control-Request-Method: $ALLOWED_METHOD" -H "Access-Control-Request-Headers: $ALLOWED_HEADER" -H 'Origin: https://invalid.com' -X OPTIONS)"

    vary_header=$(get_header 'Vary' <<< "$actual")
    assert equal "$vary_header" 'Origin,Access-Control-Request-Method,Access-Control-Request-Headers'

    status=$(get_status <<< "$actual")
    assert equal "$status" 204
  ti

  it 'rejects preflight requests with disallowed methods'
    actual="$(curl "$SERVER_ADDR" -s -i -H 'Access-Control-Request-Method: PATCH' -H "Access-Control-Request-Headers: $ALLOWED_HEADER" -H "Origin: $ALLOWED_ORIGIN" -X OPTIONS)"

    vary_header=$(get_header 'Vary' <<< "$actual")
    assert equal "$vary_header" 'Origin,Access-Control-Request-Method,Access-Control-Request-Headers'

    status=$(get_status <<< "$actual")
    assert equal "$status" 204
  ti

  it 'returns the expected headers for an OPTIONS request with an allowed Origin'
    actual="$(curl "$SERVER_ADDR" -s -i -H "Access-Control-Request-Method: $ALLOWED_METHOD" -H "Access-Control-Request-Headers: $ALLOWED_HEADER" -H "Origin: $ALLOWED_ORIGIN" -X OPTIONS)"

    vary_header=$(get_header 'Vary' <<< "$actual")
    assert equal "$vary_header" 'Origin,Access-Control-Request-Method,Access-Control-Request-Headers'

    allow_origin_header=$(get_header 'Access-Control-Allow-Origin' <<< "$actual")
    assert equal "$allow_origin_header" "$ALLOWED_ORIGIN"

    allow_methods_header=$(get_header 'Access-Control-Allow-Methods' <<< "$actual")
    assert equal "$allow_methods_header" "$ALLOWED_METHOD"

    allow_headers_header=$(get_header 'Access-Control-Allow-Headers' <<< "$actual")
    assert equal "$allow_headers_header" "$ALLOWED_HEADER,Origin"

    status=$(get_status <<< "$actual")
    assert equal "$status" 204
  ti
end_describe
