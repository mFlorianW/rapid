#!/bin/bash

THIS_PATH="$(realpath "$0")"
THIS_DIR="$(dirname "$THIS_PATH")"
FILE_LIST="$(find "$THIS_DIR" -regex '.*\.\(cpp\|hpp\)')"

clang-format --verbose -i --style=file $FILE_LIST
