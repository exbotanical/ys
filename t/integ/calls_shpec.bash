ROOT_DIR="$(dirname "$(readlink -f $BASH_SOURCE)")"

source "$ROOT_DIR/shpec_util.bash"

SERVER_ADDR='localhost:9000'

get_header () {
  curl -i -s "$SERVER_ADDR" | sed -n "s/$1: //p"
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
    actual="$(get_header 'Content-Type')"
    assert equal 'text/plain' "$actual"
  ti

  it 'sets the X-powered-by header'
    actual="$(get_header 'X-Powered-By')"
    assert equal 'integ-test' "$actual"
  ti

  it 'returns a 405 for unsupported methods'
    declare -a invalid_methods=('POST' 'PUT' 'PATCH' 'DELETE' 'HEAD' 'TRACE' 'OPTIONS' 'CONNECT')

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
