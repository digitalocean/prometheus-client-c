#!/usr/bin/env bash

autolib_check_dev(){
  local dev=${DEV:=0}
  if (( dev == 0 )); then
    autolib_output_err "Not executing in the Docker environment"
    return 1
  fi
}