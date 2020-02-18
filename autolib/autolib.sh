#!/usr/bin/env bash

lib="$(dirname ${BASH_SOURCE[0]})"

source "${lib}/output.sh"
source "${lib}/build.sh"
source "${lib}/docker.sh"
source "${lib}/env.sh"
source "${lib}/test.sh"
