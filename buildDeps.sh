#!/bin/bash

if [[ -e UtilTimeConfig.cmake ]]; then
    echo "Building dependencies..."
else
    echo "This script should be run in the home directory of the project"
    exit 1
fi

if [[ -e deps/CMakeUtils ]]; then
    echo "Existing CMakeUtils directory, no need to clone"
else
    git clone https://github.com/Grauniad/CMakeUtils.git deps/CMakeUtils || exit 1
fi

declare -A depList
depList[gtest]=https://github.com/google/googletest.git

source deps/CMakeUtils/build_tools/buildDepsCommon.sh $@ || exit 1

