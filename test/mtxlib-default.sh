#!/bin/sh
set -e
test_program="$(pwd)/mtxlib-default"
workdir=$(mktemp -d "${TMPDIR:-/tmp}/mtxlib-default.XXXXXX")
trap 'rm -rf "$workdir"' 0
trap 'exit 1' HUP INT TERM
cd "$workdir"
unset MTXLIB
"$test_program" "$MTX_DEFAULT_LIB"
"$test_program" "$MTX_DEFAULT_LIB" --
MTXLIB=/runtime/table/directory "$test_program" /runtime/table/directory --
MTXLIB=/runtime/table/directory "$test_program" /command/line/directory -L /command/line/directory
