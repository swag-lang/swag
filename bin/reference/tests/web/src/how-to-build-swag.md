# How to build Swag

## LLVM

Swag has two backends, a `x86_64` custom backend written for fast compile, but with far from optimal generated code, and `llvm` for optimized builds.

The [LLVM](https://releases.llvm.org/download.html) source tree is included in the Swag source tree for convenience. Version is `15.0.7`.

In order to build LLVM, you will have to install [cmake 3.23.2](https://cmake.org/download/) (or later) and [python 3](https://www.python.org/downloads/).

## Build

You will need `Visual Studio 2022 17.1` or later.

* As there's no automatic detection, edit `vs_build_cfg.bat` to match your version of Visual Studio and of the Windows SDK.
* Launch `swag/build/vs_build_llvm_release.bat`. Note that building LLVM takes a crazy amount of time and memory, and can require multiple tries.
* Launch `swag/build/vs_build_swag_release.bat`.
* You can also launch `swag/build/vs_build_extern.bat`. This will build and update some external libraries in the standard workspace, and copy some libraries from the windows SDK.

If LLVM has been compiled once, you can also use the `Swag.sln` workspace in the `build` subfolder.

## Windows SDK
The path to the SDK version is defined in `vs_build_cfg.bat`.

The Swag **runtime** contains a copy of some libraries from the SDK (`kernel32.lib`, `ucrt.lib`, `dbghelp.lib` and `user32.lib`). You will find them in `bin/runtime/windows-x86-64`.

They are shipped with the compiler to avoid the necessity to install the SDK before building with Swag. That way the compiler can be used "as is".

The standard modules (in `bin/std/modules`) can also have dependencies to some other libraries from the SDK. You will find those dependencies in the `publish` folder of the corresponding modules.
