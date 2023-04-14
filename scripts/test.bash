#!/usr/bin/env bash
IFS=$'\n'

TESTING_DIR=t/unit
UTIL_F=util.bash
LIB=ys

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
	gcc -o main main.o -L./ -l$LIB -lm -lpcre

	green "\n[+] Running test $file_name...\n\n"

	./main

	green "\n[+] Tests for $file_name passed\n\n"
}

main () {
	export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

	declare -a tests=($(ls $TESTING_DIR | filter not_test_file _test.c))

	for_each run_test ${tests[*]}
}

. "$(dirname "$(readlink -f "$BASH_SOURCE")")"/$UTIL_F
main $*
