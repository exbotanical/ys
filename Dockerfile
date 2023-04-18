FROM ubuntu:20.04

RUN apt-get update && apt-get install -y curl git gcc make libpcre3-dev libssl-dev

RUN sh -c "`curl -L https://raw.githubusercontent.com/rylnd/shpec/master/install.sh`"

COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
