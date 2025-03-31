# Partikle Runtime

## Overview

Partikle is a lightweight and performant JavaScript web runtime environment
that is designed to start fast and conserve resources to minimize hosting
costs.

- Provides a small core library based on modern Web Platform APIs
- Standard library with dedicated support for:
    - Storage (S3)
    - Database (KV and SQL)
    - Messaging (internal routing, queuing, and pub/sub)
- Includes a zero-config, built-in JavaScript engine and compiler toolchain
    - Performs automatic type-stripping for typed JavaScript
- Provides a unified terminal user interface that combines:
    - Command shell
    - JavaScript REPL
    - Admin console for deployment and monitoring

## Components

Commands

- ptkl - command line interface (CLI)
- ptkltest - tests

Libraries

- libcli - command line interface library
- libptkl - the core runtime C library
- libqjs - adapter for the QuickJS engine
- libstd - common data types; logging, testing

## Contributing

Prerequisites

- gcc or clang (clang is significantly faster)
- cmake
- clang-tidy (for formatting)

The project uses cmake for the build system.
However, a Makefile is provided to simplify
various build tasks.

```text

# Get source
clone https://github.com/subfuzion/ptkl
git submodule init

# Initialize build
make cmake-init

# Make debug build
make cmake-debug

# Output
# bin -> ./build/debug/bin
# lib -> ./build/debug/lib

# Watch - run only one or the other
./watchmake
./watchtest

# Clean everything
make cleanall

# Formatting code
make format

```