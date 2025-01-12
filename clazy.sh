#!/bin/bash
export CLAZY_CHECKS="level1,no-non-pod-global-static"
FILES=$(find ${1} -type f -regextype posix-extended -regex '.*\.(cpp|hpp)$')
clazy-standalone -p ${2} --ignore-included-files ${FILES}
