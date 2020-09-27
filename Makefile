
SHELL = /bin/bash

GITHUB_ORG = digitalocean

# Origin does not point to https://github.com/digitalocean/prometheus-client-c.git in TravisCI so we must add a new
# remote for fetching. Fetch master, diff on the filenames and look for C files. If no changes to C files are made, skip
# the build.
CHANGED_FILES = $(shell git remote add ci https://github.com/${GITHUB_ORG}/prometheus-client-c.git > /dev/null 2>&1; git fetch ci master > /dev/null 2>&1; git diff --name-only ci/master | egrep -v '.*\.md$$')

ifneq ($(shell echo "x${CHANGED_FILES}x" | sed 's/\n\t //'), xx)
default: build_and_test
else
default: build_and_test
# default: changed_files
	# @echo -e "\033[1;32mNothing to build\033[0m"
endif

build_and_test: changed_files clean build test package smoke
.PHONY: build_and_test

all: build_and_test docs
.PHONY: all

clean:
	./auto dev -e auto -a clean

build: clean
	./auto dev -e auto -a build -a -t
.PHONY: build

test: build
	./auto dev -e auto -a test
.PHONY: test

package: test
	./auto dev -e auto -a package
.PHONY: smoke

docs: smoke
	./auto dev -e auto -a docs
.PHONY: package

changed_files:
	@echo "Changed C files: ${CHANGED_FILES}"
.PHONY: changed_files

smoke: package
	./auto dev -e auto -a smoke
.PHONY: smoke
