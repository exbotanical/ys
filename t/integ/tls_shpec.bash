ROOT_DIR="$(dirname "$(readlink -f $BASH_SOURCE)")"

source "$ROOT_DIR/shpec_util.bash"

SERVER_ADDR='https://localhost:6124'

make USE_TLS=1
export LD_LIBRARY_PATH=.

./test_server_bin >/dev/null &

describe 'libys TLS integration tests'
  it 'GET request returns payload'
    res="$(curl "$SERVER_ADDR")"
    assert equal 'Hello World!' "$res"
  ti

  it 'GET request returns 200 status'
    res="$(curl -s -i "$SERVER_ADDR")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'
  ti

  it 'sets the Content-Type header'
    res="$(curl -i -s "$SERVER_ADDR" | get_header 'Content-Type')"
    assert equal 'text/plain' "$res"
  ti

  it 'sets multiple X-powered-by headers'
    res="$(curl -i -s "$SERVER_ADDR" | get_header 'X-Powered-By')"
    assert equal 'libys, demo, integ-test' "$res"
  ti

  it 'returns a 405 for unsupported methods'
    declare -a invalid_methods=('POST' 'PUT' 'PATCH' 'DELETE' 'HEAD' 'TRACE' 'CONNECT')

    for method in "${invalid_methods[@]}"; do
      res="$(curl -s -i -X "$method" "$SERVER_ADDR")"

      status="$(get_status <<< "$res")"
      assert equal "$status" '405 Method Not Allowed'
    done
  ti

  it 'returns 404 for invalid routes'
    res="$(curl -s -i "$SERVER_ADDR/invalid")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '404 Not Found'
  ti

  it 'handles a request with duplicate headers'
    res="$(curl -s -i "$SERVER_ADDR" -H 'header:v' -H 'header:v2')"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'
  ti

  it 'GET request to nested path returns payload'
    res="$(curl "$SERVER_ADDR/api")"
    assert equal 'api root' "$res"
  ti
end_describe

kill $!
