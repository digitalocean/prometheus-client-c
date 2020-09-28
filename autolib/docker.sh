#!/usr/bin/env bash

lib="$(dirname ${BASH_SOURCE[0]})"
source "${lib}/output.sh"

PROJECT_ROOT=$(pushd "$(dirname ${BASH_SOURCE[0]})/.." > /dev/null; echo $PWD; popd > /dev/null)

autolib_new_debian_template(){
  cat <<'EOF'
FROM __DOCKER_IMAGE__

RUN set -x && \
    apt-get update && \
    apt-get install -y apt-utils software-properties-common clang-format && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update -y && \
    apt-get install -y curl tar build-essential git pkg-config gdb valgrind gcc-10 libmicrohttpd-dev doxygen graphviz && \
    rm -f /usr/bin/gcc && \
    ln -s /usr/bin/gcc-10 /usr/bin/gcc && \
    curl -sL https://github.com/Kitware/CMake/releases/download/v3.14.5/cmake-3.14.5-Linux-x86_64.tar.gz | tar xzf - -C /opt && \
    cp /opt/cmake-3.14.5-Linux-x86_64/bin/* /usr/local/bin/ && \
    cp -R /opt/cmake-3.14.5-Linux-x86_64/share/cmake-3.14 /usr/local/share/ && \
    curl -sL https://dl.google.com/go/go1.13.1.linux-amd64.tar.gz 2> /dev/null | tar xzf - -C /usr/local && \
    mkdir -p /gopath/{src,bin} && \
    printf 'export GOPATH=/gopath\nexport PATH=$PATH:/usr/local/go/bin:/gopath/bin\n' > /root/.bash_profile && \
    printf '#!/usr/bin/env bash\nsource /root/.bash_profile\nexec /bin/bash $@\n' > /entrypoint && \
    chmod +x /entrypoint && \
    GOPATH=/gopath /usr/local/go/bin/go get github.com/prometheus/prom2json && \
    GOPATH=/gopath /usr/local/go/bin/go install github.com/prometheus/prom2json/cmd/prom2json && \
    GOPATH=/gopath /usr/local/go/bin/go get github.com/git-chglog/git-chglog && \
    GOPATH=/gopath /usr/local/go/bin/go install github.com/git-chglog/git-chglog/cmd/git-chglog && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /code
ENTRYPOINT ["/entrypoint"]

EOF
}

autolib_old_debian_template(){
  cat <<'EOF'
FROM __DOCKER_IMAGE__

ENV GCC_VERSION 10.1.0

RUN set -x && \
    apt-get update && \
    apt-get install -y apt-utils clang-format && \
    apt-get install -y curl tar build-essential git pkg-config gdb valgrind gcc libmicrohttpd-dev doxygen graphviz && \
    curl -sL https://github.com/Kitware/CMake/releases/download/v3.14.5/cmake-3.14.5-Linux-x86_64.tar.gz | tar xzf - -C /opt && \
    cp /opt/cmake-3.14.5-Linux-x86_64/bin/* /usr/local/bin/ && \
    cp -R /opt/cmake-3.14.5-Linux-x86_64/share/cmake-3.14 /usr/local/share/ && \
    curl -sL https://dl.google.com/go/go1.13.1.linux-amd64.tar.gz 2> /dev/null | tar xzf - -C /usr/local && \
    mkdir -p /gopath/{src,bin} && \
    printf 'export GOPATH=/gopath\nexport PATH=$PATH:/usr/local/go/bin:/gopath/bin\n' > /root/.bash_profile && \
    printf '#!/usr/bin/env bash\nsource /root/.bash_profile\nexec /bin/bash $@\n' > /entrypoint && \
    chmod +x /entrypoint && \
    GOPATH=/gopath /usr/local/go/bin/go get github.com/prometheus/prom2json && \
    GOPATH=/gopath /usr/local/go/bin/go install github.com/prometheus/prom2json/cmd/prom2json && \
    GOPATH=/gopath /usr/local/go/bin/go get github.com/git-chglog/git-chglog && \
    GOPATH=/gopath /usr/local/go/bin/go install github.com/git-chglog/git-chglog/cmd/git-chglog && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /code
ENTRYPOINT ["/entrypoint"]

EOF
}

autolib_debian_jessie_template() {
  cat <<EOF
FROM __DOCKER_IMAGE__

ENV GCC_VERSION 10.1.0

RUN set -x && \
    apt-get update && \
    apt-get install -y apt-utils && \
    apt-get install -y curl tar build-essential git pkg-config gdb valgrind gcc libmicrohttpd-dev doxygen graphviz && \
    curl -sL https://github.com/Kitware/CMake/releases/download/v3.14.5/cmake-3.14.5-Linux-x86_64.tar.gz | tar xzf - -C /opt && \
    cp /opt/cmake-3.14.5-Linux-x86_64/bin/* /usr/local/bin/ && \
    cp -R /opt/cmake-3.14.5-Linux-x86_64/share/cmake-3.14 /usr/local/share/ && \
    curl -sL https://dl.google.com/go/go1.13.1.linux-amd64.tar.gz 2> /dev/null | tar xzf - -C /usr/local && \
    mkdir -p /gopath/{src,bin} && \
    printf 'export GOPATH=/gopath\nexport PATH=$PATH:/usr/local/go/bin:/gopath/bin\n' > /root/.bash_profile && \
    printf '#!/usr/bin/env bash\nsource /root/.bash_profile\nexec /bin/bash $@\n' > /entrypoint && \
    chmod +x /entrypoint && \
    GOPATH=/gopath /usr/local/go/bin/go get github.com/prometheus/prom2json && \
    GOPATH=/gopath /usr/local/go/bin/go install github.com/prometheus/prom2json/cmd/prom2json && \
    GOPATH=/gopath /usr/local/go/bin/go get github.com/git-chglog/git-chglog && \
    GOPATH=/gopath /usr/local/go/bin/go install github.com/git-chglog/git-chglog/cmd/git-chglog && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /code
ENTRYPOINT ["/entrypoint"]


EOF
}

autolib_write_dockerfile(){
  local docker_image="$1"
  local r
  case "$docker_image" in
    ( ubuntu:20.04 | ubuntu:18.04 ) {
       autolib_new_debian_template | sed "s/__DOCKER_IMAGE__/$docker_image/g" > ${PROJECT_ROOT}/docker/Dockerfile || {
        r=$?
        autolib_output_error "failed to generate dockerfile"
        return $r
      }
    } ;;
    ( ubuntu:16.04 | debian:buster | debian:stretch ) {
      autolib_old_debian_template | sed "s/__DOCKER_IMAGE__/$docker_image/g" > ${PROJECT_ROOT}/docker/Dockerfile || {
        r=$?
        autolib_output_error "failed to generate dockerfile"
        return $r
      }
    } ;;
    ( debian:jessie ) {
       autolib_debian_jessie_template | sed "s/__DOCKER_IMAGE__/$docker_image/g" > ${PROJECT_ROOT}/docker/Dockerfile || {
        r=$?
        autolib_output_error "failed to generate dockerfile"
        return $r
      }
    } ;;
    ( * ) {
      r=1
      autolib_output_error "unsupported DOCKER_IMAGE: $docker_image"
      return $r
    } ;;
  esac
}