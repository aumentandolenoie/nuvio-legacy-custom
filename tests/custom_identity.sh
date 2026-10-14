#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

# Simulate GCC's response-file compilation used by tools/arm.sh.
printf '%s\n' '-DNV_CUSTOM_BUILD' > "$work/custom.rsp"
cc @"$work/custom.rsp" tests/custom_identity.c -o "$work/custom-id"
"$work/custom-id"

# Standard build must keep its original ID.
cc tests/custom_identity.c -o "$work/legacy-id"
"$work/legacy-id"

# Catch an accidental reintroduction of the broken -DNV_APP_ID=...
if grep '^export NUVIO_EXTRA_CFLAGS=' tools/build-custom.sh | grep -q -- '-DNV_APP_ID'; then
  echo 'FAIL: Custom ID must be declared in app_id.h, not passed via -D flag'
  exit 1
fi
