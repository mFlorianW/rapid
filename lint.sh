#!/bin/bash

FILES=$(find ${1} -type f -regextype posix-extended -regex '.*\.(cpp|hpp)$')
run-clang-tidy -j $(nproc --all) -p "${2}" ${FILES}
