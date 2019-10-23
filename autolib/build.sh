#!/usr/bin/env bash

lib="$(dirname ${BASH_SOURCE[0]})"

source "${lib}/env.sh"
source "${lib}/output.sh"

# build all libraries
autolib_build() {
  local lib=$1
  local build_test=$2

  if [[ ! -d ${lib}/build ]]; then
    mkdir ${lib}/build || {
      autolib_output_error "Failed to create build directory"
      return 1
    }
  fi
  pushd ${lib}/build > /dev/null || return $?
    autolib_output_banner "${lib}: CMake Build Stage"
    # YOU MUST set TEST to 1 in order to build the tests
    TEST=$build_test cmake -v .. || {
      autolib_output_error "${lib}: CMake Failure"
      return 1
    }
    autolib_output_banner "${lib}: Make Build Stage"
    make || {
      autolib_output_error "${lib} Make Failure"
      return 1
    }
  popd > /dev/null || return $?
}