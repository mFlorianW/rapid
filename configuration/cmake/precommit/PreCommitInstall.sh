#!/bin/bash
if [[ "$(which pre-commit)" == "" ]]; then
    pip install --user pre-commit
fi
pre-commit install
