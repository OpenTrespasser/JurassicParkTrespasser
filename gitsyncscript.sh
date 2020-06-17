#!/bin/sh
git submodule update --init
pushd jp2_pc/BuildTools/CMake
git-lfs fetch
popd
