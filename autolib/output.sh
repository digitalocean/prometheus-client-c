#!/usr/bin/env bash

# Print a stage. Pass a string to be enveloped in a banner.
autolib_output_banner(){
  echo -e "\n\033[1m########################################################################\033[0m" >&2
  echo -e "\033[1m> $@\033[0m" >&2
  echo -e "\033[1m########################################################################\033[0m\n" >&2
}

autolib_output_error() {
  echo -e "\033[1;31mERROR: $@\033[0m" >&2
}

autolib_output_success() {
  echo -e "\033[1;32mSUCCESS: $@\033[0m" >&2
}

autolib_output_warn() {
  echo -e "\033[1;33mWARN: $@\033[0m" >&2
}