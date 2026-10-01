#!/bin/sh
set -e
unset MTXLIB
./mtxlib-default "$MTX_DEFAULT_LIB"
./mtxlib-default "$MTX_DEFAULT_LIB" --
MTXLIB=/runtime/table/directory ./mtxlib-default /runtime/table/directory --
MTXLIB=/runtime/table/directory ./mtxlib-default /command/line/directory -L /command/line/directory
