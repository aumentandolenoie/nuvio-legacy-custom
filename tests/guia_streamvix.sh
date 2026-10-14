#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
work=$(mktemp -d /tmp/nuvio-guia-streamvix.XXXXXX)
trap 'rm -rf "$work"' EXIT
flags=(-ffunction-sections -fdata-sections)
if [ "$(uname -s)" = Darwin ]; then flags+=(-Wl,-dead_strip); else flags+=(-Wl,--gc-sections); fi
cc tests/guia_streamvix.c src/js.c -Isrc \
  -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 \
  -O1 -g -Wall -Wno-unused-function -Wno-macro-redefined "${flags[@]}" -o "$work/test"
"$work/test"
