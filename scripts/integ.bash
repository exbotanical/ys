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
  ./integ_test &
  pid=$!
  sleep 1

	declare -a tests=(
    $(ls $TESTING_DIR | filter not_test_file _shpec.bash | grep -v 'tls_')
  )

	for_each run_test ${tests[*]}

  quietly_kill $pid
}

run_ssl () {
  ./integ_test USE_SSL &
  pid=$!
  sleep 1

  declare -a tests=(
    $(ls $TESTING_DIR | filter not_test_file _shpec.bash | grep 'tls_')
  )

	for_each run_test ${tests[*]}

  quietly_kill $pid
}

main () {
  kill $(pgrep integ_test) 2>/dev/null ||:

  export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:."
  [[ "$OSTYPE" == "darwin"* ]] && {
    export DYLD_LIBARY_PATH="$DYLD_LIBARY_PATH:$LD_LIBRARY_PATH"
  }

  run
  run_ssl
}

. "$(dirname "$(readlink -f "$BASH_SOURCE")")"/$UTIL_F
main $*
