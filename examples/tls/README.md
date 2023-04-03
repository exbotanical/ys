# TLS Example

This example demonstrates a simple server with TLS. In order to enable TLS, you'll need to make the library by running `make USE_TLS=1` from the root directory, then `make` this demo application and run the server with `./run.sh`.

Try sending a request with `curl https://localhost:9000/`. Also try out `curl localhost:9000/` to see how the framework handles an invalid protocol.
