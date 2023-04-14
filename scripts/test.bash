#!/usr/bin/env bash
IFS=$'\n'

TESTING_DIR=t/unit
UTIL_F=util.bash
LIB=ys
LD_FLAGS="-L./ -l$LIB -lm -lpcre -lcrypto -lssl"

declare -a SKIP_FILES=(
  # 'cache_test.c'
  # 'config_test.c'
  # 'cookie_test.c'
  # 'cors_test.c'
  # 'enum_test.c'
  # 'header_test.c'
  # 'ip_test.c'
  # 'json_test.c'
  # 'middleware_test.c'
	# 'path_test.c'
  # 'request_test.c'
  # 'response_test.c'
	# 'trie_test.c'
  # 'url_test.c'
  # 'util_test.c'
)

run_test () {
	local file_name="$1"

	gcc -w -Ideps -Isrc -Ilib -c "$TESTING_DIR/$file_name" -o main.o
	eval "gcc -o main main.o $LD_FLAGS"

	green "\n[+] Running test $file_name...\n\n"

	./main

	green "\n[+] Tests for $file_name passed\n\n"
}

main () {
  export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:."
  [[ "$OSTYPE" == "darwin"* ]] && {
    export DYLD_LIBARY_PATH="$DYLD_LIBARY_PATH:$LD_LIBRARY_PATH"
    LD_FLAGS="$LD_FLAGS -L/usr/local/include/"
  }

	declare -a tests=($(ls $TESTING_DIR | filter not_test_file _test.c))

	for_each run_test ${tests[*]}
}

. "$(dirname "$(readlink -f "$BASH_SOURCE")")"/$UTIL_F
main $*
