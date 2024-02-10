# How to build Swag

## LLVM

Swag has two backends, a `x86_64` custom backend written for fast compile, but with far from optimal generated code, and `llvm` for optimized builds.

The [LLVM](https://releases.llvm.org/download.html) source tree is included in the Swag source tree for convenience. Version is `17.0.6`.

In order to build LLVM, you will have to install [cmake 3.23.2](https://cmake.org/download/) (or later) and [python 3](https://www.python.org/downloads/).

## Build

You will need `Visual Studio 2022 17.1` or later.

* As there's no automatic detection, edit `vs_build_cfg.bat` to match your version of Visual Studio.
* Launch `swag/build/vs_build_llvm_release.bat`. Note that building LLVM takes a crazy amount of time and memory.
* Launch `swag/build/vs_build_swag_release.bat`.
* You can also launch `swag/build/vs_build_extern.bat`. This will build and update some external libraries in the standard workspace.

If LLVM has been compiled once, you can also use the `Swag.sln` workspace in the `build` subfolder.

