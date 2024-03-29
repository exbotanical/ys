ROOT_DIR="$(dirname "$(readlink -f $BASH_SOURCE)")"

source "$ROOT_DIR/shpec_util.bash"

SERVER_ADDR='localhost:6124'

describe 'libys basic integration tests'
  it 'GET request returns payload'
    res="$(curl "$SERVER_ADDR")"
    assert equal 'Hello World!' "$res"
  ti

  it 'GET request returns 200 status'
    res="$(curl -s -i "$SERVER_ADDR")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'
  ti

  it 'sets the Content-Type header to text/plain by default if a body is present'
    res="$(curl -i -s "$SERVER_ADDR" | get_header 'Content-Type')"
    assert equal 'text/plain' "$res"
  ti

  it 'sets multiple X-powered-by headers'
    res="$(curl -i -s "$SERVER_ADDR" | get_header 'X-Powered-By')"
    assert equal 'Ys,demo,integ-test' "$res"
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
end_describe

describe 'CORS functionality'
  ALLOWED_ORIGIN='test.com'
  ALLOWED_METHOD='DELETE'
  ALLOWED_HEADER='X-Test-Header'

  it 'rejects preflight requests with disallowed headers'
    res="$(curl "$SERVER_ADDR" -s -i -H "Access-Control-Request-Method: $ALLOWED_METHOD" -H 'Access-Control-Request-Headers: X-Requested-With' -H "Origin: $ALLOWED_ORIGIN" -X OPTIONS)"

    vary_header=$(get_header 'Vary' <<< "$res")
    assert equal "$vary_header" 'Origin,Access-Control-Request-Method,Access-Control-Request-Headers'

    status=$(get_status <<< "$res")
    assert equal "$status" '204 No Content'
  ti

  it 'rejects preflight requests with disallowed origins'
    res="$(curl "$SERVER_ADDR" -s -i -H "Access-Control-Request-Method: $ALLOWED_METHOD" -H "Access-Control-Request-Headers: $ALLOWED_HEADER" -H 'Origin: https://invalid.com' -X OPTIONS)"

    vary_header=$(get_header 'Vary' <<< "$res")
    assert equal "$vary_header" 'Origin,Access-Control-Request-Method,Access-Control-Request-Headers'

    status=$(get_status <<< "$res")
    assert equal "$status" '204 No Content'
  ti

  it 'rejects preflight requests with disallowed methods'
    res="$(curl "$SERVER_ADDR" -s -i -H 'Access-Control-Request-Method: PATCH' -H "Access-Control-Request-Headers: $ALLOWED_HEADER" -H "Origin: $ALLOWED_ORIGIN" -X OPTIONS)"

    vary_header=$(get_header 'Vary' <<< "$res")
    assert equal "$vary_header" 'Origin,Access-Control-Request-Method,Access-Control-Request-Headers'

    status=$(get_status <<< "$res")
    assert equal "$status" '204 No Content'
  ti

  it 'returns the expected headers for an OPTIONS request with an allowed Origin'
    res="$(curl -s -i "$SERVER_ADDR" -H "Access-Control-Request-Method: $ALLOWED_METHOD" -H "Access-Control-Request-Headers: $ALLOWED_HEADER" -H "Origin: $ALLOWED_ORIGIN" -X OPTIONS)"

    vary_header=$(get_header 'Vary' <<< "$res")
    assert equal "$vary_header" 'Origin,Access-Control-Request-Method,Access-Control-Request-Headers'

    allow_origin_header=$(get_header 'Access-Control-Allow-Origin' <<< "$res")
    assert equal "$allow_origin_header" "$ALLOWED_ORIGIN"

    allow_methods_header=$(get_header 'Access-Control-Allow-Methods' <<< "$res")
    assert equal "$allow_methods_header" "$ALLOWED_METHOD"

    allow_headers_header=$(get_header 'Access-Control-Allow-Headers' <<< "$res")
    assert equal "$allow_headers_header" "$ALLOWED_HEADER,Origin"

    status=$(get_status <<< "$res")
    assert equal "$status" '204 No Content'
  ti
end_describe

describe 'REST functionality'
  it 'returns a 404 when no matching record'
    res="$(curl -s -i "$SERVER_ADDR/records/2")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '404 Not Found'

    body="$(tail -1 <<< "$res")"
    assert equal "$body" '{"message":"no matching record"}'
  ti

  it 'creates a new record on POST and returns 201'
    res="$(curl -s -i "$SERVER_ADDR/records/2" -H 'Content-Type: application/json' -d '{"v":"someval"}')"

    status="$(get_status <<< "$res")"
    assert equal "$status" '201 Created'
  ti

  it 'returns a 400 when trying to POST a record that already exists'
    res="$(curl -s -i "$SERVER_ADDR/records/2" -H 'Content-Type: application/json' -d '{"v":"someval"}')"

    status="$(get_status <<< "$res")"
    assert equal "$status" '400 Bad Request'

    body="$(tail -1 <<< "$res")"
    assert equal "$body" '{"message":"record exists"}'
  ti

  it 'GETS the newly created record data in the response body'
    res="$(curl -s -i "$SERVER_ADDR/records/2")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'

    body="$(tail -1 <<< "$res")"
    assert equal "$body" '{"data":"{"key":"2","value":"someval"}"}'
  ti

  it 'updates the data with PUT and returns 200'
    res="$(curl -s -i "$SERVER_ADDR/records/2" -X PUT -H 'Content-Type: application/json' -d '{"v": "updatedval"}' )"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'
  ti

  it 'GET retrieves the data'
    res="$(curl -s -i "$SERVER_ADDR/records/2")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'

    body="$(tail -1 <<< "$res")"
    assert equal "$body" '{"data":"{"key":"2","value":"updatedval"}"}'
  ti

  it 'DELETE deletes the record and returns 200'
    res="$(curl -s -i "$SERVER_ADDR/records/2" -X DELETE)"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'
  ti

  it 'GET returns no match with a 404'
    res="$(curl -s -i "$SERVER_ADDR/records/2")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '404 Not Found'

    body="$(tail -1 <<< "$res")"
    assert equal "$body" '{"message":"no matching record"}'
  ti
end_describe

describe 'query and route parameters'
  it 'reads all of the query parameters'
    res="$(curl -s -i "$SERVER_ADDR/metadata/?movies=LoveExposure&movies=Downtown81")"

    body="$(tail -1 <<< "$res")"
    assert equal "$body" 'LoveExposureDowntown81'
  ti
end_describe

describe 'sub-router functionality'
  it 'GET request to nested path returns payload'
    res="$(curl "$SERVER_ADDR/api")"
    assert equal 'api root' "$res"
  ti

  it 'GET request to nested path returns 200 status'
    res="$(curl -s -i "$SERVER_ADDR/api")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'
  ti

  it 'POST request to nested path returns payload'
    expected='the body'
    res="$(curl "$SERVER_ADDR/api/demo" -d "$expected")"

    assert equal "$expected" "$res"
  ti

  it 'POST request to nested path returns 200 status'
    res="$(curl -s -i "$SERVER_ADDR/api/demo" -d "$expected")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '200 OK'
  ti

  it 'returns a 405 for unsupported methods'
    declare -a invalid_methods=('POST' 'PUT' 'PATCH' 'DELETE' 'HEAD' 'TRACE' 'CONNECT')

    for method in "${invalid_methods[@]}"; do
      res="$(curl -s -i -X "$method" "$SERVER_ADDR/api")"

      status="$(get_status <<< "$res")"
      assert equal "$status" '405 Method Not Allowed'
    done
  ti

  it 'returns 404 for invalid routes'
    res="$(curl -s -i "$SERVER_ADDR/api/invalid")"

    status="$(get_status <<< "$res")"
    assert equal "$status" '404 Not Found'
  ti
end_describe

describe 'middleware'
  it 'skips the middleware on ignored paths'
    res="$(curl -i -s "$SERVER_ADDR/ignore" | get_header 'X-Middleware')"
    assert equal '' "$res"
  ti

  it 'skips the middleware on ignored paths (no end regex)'
    res="$(curl -i -s "$SERVER_ADDR/ignoreblablabla" | get_header 'X-Middleware')"
    assert equal '' "$res"
  ti


  it 'triggers the middleware on non-ignored paths'
    res="$(curl -i -s "$SERVER_ADDR/" | get_header 'X-Middleware')"
    assert equal 'test' "$res"
  ti

  it 'allows OPTIONS requests for CORS but does not fall through to route'
    res="$(curl -i -s "$SERVER_ADDR/" -X OPTIONS | get_header 'X-Root-Handler')"
    assert equal '' "$res"
  ti
end_describe
