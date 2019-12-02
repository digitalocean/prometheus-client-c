FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y apt-utils && \
    apt-get install -y curl tar build-essential git pkg-config gdb valgrind gcc libmicrohttpd-dev doxygen graphviz && \
    curl -L https://dl.google.com/go/go1.13.1.linux-amd64.tar.gz 2> /dev/null | tar xzvf - -C /usr/local && \
    mkdir -p /gopath/{src,bin} && \
    printf 'export GOPATH=/gopath\nexport PATH=$PATH:/usr/local/go/bin:/gopath/bin\n' > /root/.bash_profile && \
    printf '#!/usr/bin/env bash\nsource /root/.bash_profile\nexec /bin/bash $@\n' > /entrypoint && \
    chmod +x /entrypoint && \
    GOPATH=/gopath /usr/local/go/bin/go get github.com/prometheus/prom2json && \
    GOPATH=/gopath /usr/local/go/bin/go install github.com/prometheus/prom2json/cmd/prom2json && \
    GOPATH=/gopath /usr/local/go/bin/go get github.com/git-chglog/git-chglog && \
    GOPATH=/gopath /usr/local/go/bin/go install github.com/git-chglog/git-chglog/cmd/git-chglog && \
    curl -L https://github.com/Kitware/CMake/releases/download/v3.15.0-rc1/cmake-3.15.0-rc1.tar.gz | tar xzvf - -C /tmp/ && \
    cd /tmp/cmake-3.15.0-rc1 && ./bootstrap && make && make install && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /code
ENTRYPOINT ["/entrypoint"]