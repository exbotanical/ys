#!/usr/bin/env bash
IFS=$'\n'

TESTING_DIR=t/integ
UTIL_F=util.bash

declare -a SKIP_FILES=(
  # 'auth_shpec.bash'
  # 'basic_shpec.bash'
  # 'tls_shpec.bash'
)

run_test () {
	local file_name="$1"

  shpec "$TESTING_DIR/$file_name"
}

run () {
  ./test_server_bin &
  pid=$!

	declare -a tests=(
    $(ls $TESTING_DIR | filter not_test_file _shpec.bash | grep -v 'tls_')
  )

	for_each run_test ${tests[*]}

  quietly_kill $pid
}

run_ssl () {
  ./test_server_bin USE_SSL &
  pid=$!

  declare -a tests=(
    $(ls $TESTING_DIR | filter not_test_file _shpec.bash | grep 'tls_')
  )

	for_each run_test ${tests[*]}

  quietly_kill $pid
}

main () {
  export LD_LIBRARY_PATH=.

  run

  run_ssl
}

. "$(dirname "$(readlink -f "$BASH_SOURCE")")"/$UTIL_F
main $*
