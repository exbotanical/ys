#!/usr/bin/env bash
IFS=$'\n'

TESTING_DIR=t/integ
UTIL_F=util.bash

declare -a SKIP_FILES=(
  # 'auth_shpec.bash'
  # 'basic_shpec.bash'
  'tls_shpec.bash'
)

run_test () {
	local file_name="$1"

  shpec "$TESTING_DIR/$file_name"
}

main () {
  export LD_LIBRARY_PATH=.
  ./test_server_bin &
  trap 'echo Cleaning up...; kill $!' EXIT

	declare -a tests=($(ls $TESTING_DIR | filter not_test_file _shpec.bash))

	for_each run_test ${tests[*]}
}

. "$(dirname "$(readlink -f "$BASH_SOURCE")")"/$UTIL_F
main $*
