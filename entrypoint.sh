#!/bin/sh -l

ret="$(make -s unit_test 2>/dev/null)"

echo "$ret"

if [ "$(echo "$ret" | grep 'not ok')" != "" ]; then
  exit 1
fi

make -s integ_test 2>/dev/null
exit $?
