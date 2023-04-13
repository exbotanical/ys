ROOT_DIR="$(dirname "$(readlink -f $BASH_SOURCE)")"

source "$ROOT_DIR/shpec_util.bash"

SERVER_ADDR='localhost:6124'

export LD_LIBRARY_PATH=.

./test_server_bin >/dev/null &

COOKIE='TestCookie=100; Expires=Sat, 01 Apr 2023 02:02:26 GMT'

describe 'libys auth integration tests'
  it 'returns a 401 when not authenticated'
    res="$(curl -s -i "$SERVER_ADDR/auth/data")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '401 Unauthorized'
  ti

  it 'returns a Cookie on successful login'
    res="$(curl -s -i "$SERVER_ADDR/auth/login" -d 'x')"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'

    cookie="$(get_cookie <<< "$res")"
    assert egrep "$cookie" '.*TestCookie=.*; Expires=.*GMT*'
  ti

  it 'returns a 200 and payload of the auth-protected data when using a valid Cookie'
    res="$(curl -s -i "$SERVER_ADDR/auth/data" -H "Cookie: $COOKIE" )"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'

    data=$(grep data <<< "$res")
    assert equal "$data" '{ "data": "Hello World!" }'
  ti

  it 'invalidates the Cookie on logout by setting Max-Age to 0'
    res="$(curl -s -i "$SERVER_ADDR/auth/logout" -X POST -H "Cookie: $COOKIE" )"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'

    cookie="$(get_cookie <<< "$res")"
    assert egrep "$cookie" 'Max-Age=0'
  ti
end_describe

kill $!
