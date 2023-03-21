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
  it 'returns the expected headers for an OPTIONS request'
    actual="$(curl "$SERVER_ADDR" -v -H 'Access-Control-Request-Method: POST' -H 'Access-Control-Request-Headers: X-Requested-With' -H 'Origin: hello.com' -X OPTIONS)"
    assert equal 'TODO' 'TODO'
  ti
end_describe
