#!/usr/bin/env bash
IFS=$'\n'

TESTING_DIR=t/unit
UTIL_F=util.bash
REPO_DIR=http

declare -a SKIP_FILES=(
	'path_test.c'
	'trie_test.c'
  'util_test.c'
  'request_test.c'
  # 'header_test.c'
)

not_test_file () {
	local test=$1
	local ret=0

	for (( i=0; i < ${#SKIP_FILES[@]}; i++ )); do
		if [[ $test == ${SKIP_FILES[i]} ]]; then
			ret=1
			break
		fi
	done

	return $ret
}

run_test () {
	local file_name="$1"

	gcc -D debug -Ideps -Isrc -c "$TESTING_DIR/$file_name" -o main.o
	gcc -D debug -o main main.o -L./ -l$REPO_DIR -lm

	export LD_LIBRARY_PATH=$(pwd)/src/:$(pwd)/include/:$LD_LIBRARY_PATH
	green "\n[+] Running test $file_name...\n\n"

	./main

	green "\n[+] Tests for $file_name passed\n\n"
}

main () {
	make

	declare -a tests=($(ls $TESTING_DIR | filter not_test_file))

	for_each run_test ${tests[*]}
}

. "$(dirname "$(readlink -f "$BASH_SOURCE")")"/$UTIL_F
main $*
