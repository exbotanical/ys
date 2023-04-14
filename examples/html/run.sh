#!/usr/bin/env sh

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../../"
[[ "$OSTYPE" == "darwin"* ]] && {
  export DYLD_LIBARY_PATH="$DYLD_LIBARY_PATH:$LD_LIBRARY_PATH"
}

./demo
