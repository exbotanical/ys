#!/usr/bin/env sh

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:../../"
[[ "$OSTYPE" == "darwin"* ]] && {
  export DYLD_LIBRARY_PATH="$DYLD_LIBRARY_PATH:$LD_LIBRARY_PATH"
}

./demo
