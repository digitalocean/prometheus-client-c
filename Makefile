
SHELL = /bin/bash

build_and_test: docker clean build test package smoke
.PHONY: build_and_test

all: build_and_test docs
.PHONY: all

docker:
	pushd docker && make
.PHONY: docker

clean: docker
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

smoke: package
	./auto dev -e auto -a smoke
.PHONY: smoke

docs: smoke
	./auto dev -e auto -a docs
.PHONY: package
