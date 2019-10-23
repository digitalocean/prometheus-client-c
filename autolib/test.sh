#!/usr/bin/env bash

lib="$(dirname ${BASH_SOURCE[0]})"

source "${lib}/output.sh"

autolib_test() {
  local library="$1"
  local r
  pushd ${lib}/build > /dev/null || return $?
    autolib_output_banner "${library}: Starting Tests"
    ARGS=' --track-origins=yes --output-on-failure=yes --verbose' make test && {
      r=$?
      autolib_output_success "${library}: Test Pass"
    } || {
      r=$?
      autolib_output_error "${library}: Test Failure"
    }
  popd > /dev/null || return $?
  return $r
}

autolib_mem_test() {
  local library="$1"
  local r
  pushd ${library} > /dev/null || return $?
    autolib_output_banner "${library}: Starting MemTests"
    while read test_file; do
      valgrind --leak-check=full --error-exitcode=1 --num-callers=30 "$test_file" && {
        r=$?
        autolib_output_success "${library} ${test_file}: MemTest Pass"
      } || {
        r=$?
        autolib_output_error "${library} ${test_file}: MemTest Failure"
      }
    done < <(find build -maxdepth 1 -regex ".*prom.*test$")
  popd > /dev/null || return $?
  return $r
}