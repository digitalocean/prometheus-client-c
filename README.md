# prometheus-client-c

[![Build Status](https://github.com/digitalocean/prometheus-client-c/workflows/CI/badge.svg)](https://github.com/digitalocean/prometheus-client-c/actions)

This project provides shared libraries for instrumenting software via [Prometheus](https://prometheus.io)

* libprom - Provides the core API. Resources such as counters, gauges, histograms, and
  collector registries can be found here. This library has no dependencies on third-party
  libraries; however, it does rely on pthreads native to POSIX systems.
* libpromhttp - Provides a simple web handler to expose Prometheus metrics for scraping.
  This library has a dependency on libmicrohttpd.

Documentation can be found
[at the documentation site](https://digitalocean.github.io/prometheus-client-c/)
and an example can be found under example/. Check under the releases tab for tarballs and debian installers

## Versioning

This project generally follows [semantic versioning](https://semver.org). For each version you can find a corresponding
release under the releases type. Do not expect the master branch to maintain alignment with the latest release. It may
contain changes not yet released.

## Development

The only development dependencies required to get started are Docker, Make and Bash. Navigate to the root fo this
project directory and execute `make`. This process will build the development container, build libprom and libpromhttp,
run the unit tests and execute the smoke tests.

The stages of the development workflow are automated via `auto` which can be found in the root of this project directory.
Execute `bash auto -h` for information regarding the different subcommands. Information for each subcommand can be
obtained by executing `bash auto CMD -h`.

## Contributing

Thank you for your interest in contributing to prometheus-client-c! There two primary ways to get involved with this
project: documentation and code modifications.  In either case, the process is identical.

### General Rules for Contribution

* Open An Issue: Before opening a PR or starting any work, open an issue.  In the issue, describe the problem you
want to fix and how you would like to fix it.  The level of detail should match the relative size of the proposed change.
This will allow us to work together to determine the best path forward towards a sound solution.

* Open a Pull Request: After you have gotten confirmation on your proposed change it's time to get to work! Create a
fork and make all of your updates in said fork. For each commit, you must prefix the commit with the associated issue.
For example: `#12 - Fixing typo in documentation`. Before opening a pull request, review the commit log for your fork.
If any of your commit messages are extraneous, squash said commits using `git rebase`. Once you're happy with your
changes and your commit log, open a pull request against the master branch.

* Engage in the Code Review: After submitting your pull request, there may be some requests for changes.  If you have
any questions or concerns, please do not hesitate to make them known.  The code review process is a bidirectional
communication process so please do not be shy. Speak up!

### Coding Rules for Contribution

* Please follow the general coding style already present in the project.
  * clang-format your code by executing `bash auto format` before submitting a PR.
  * Every struct must have a constructor function and destructor function.
  * Every method must pass a pointer to the target struct as the first argument.
  * Every function that is not a constructor or destructor and does not return a value must return an int to signify
    success with 0 and failure with non-zero.
  * Every function name must begin with the library name. For example, all functions within prom must begin with `prom_`
    and all functions within promhttp must begin with `promhttp_`.
  * All variables must be underscore delimited (i.e. snake-case).
  * All macros must be captilalized.
  * Every pointer reference must be assigned a NULL value after it is destroyed either by prom_free or its
    corresponding destructor function.
* All new functions must introduce a corresponding suite of unit tests.
* If you add functionality to an existing function, be sure to add a corresponding unit test to verify correctness.

## Misc

* Language level: C11
* Operating Systems:
  * Ubuntu 20.04
  * Ubuntu 18.04
  * Ubuntu 16.04
  * Debian Buster
  * Debian Stretch
  * Debian Jessie
