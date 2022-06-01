# How to build Swag

## LLVM

Swag has two backends, a `x86_64` custom backend written for fast compile, but with far from optimal generated code, and `llvm` for optimized builds.

The LLVM source tree is included in the Swag source tree for convenience. Version is `12.0.1`

https://releases.llvm.org/download.html

In order to build LLVM, you will have to install `cmake 3.23.2` or later (https://cmake.org/download/) and `python 3` (https://www.python.org/downloads/)

## Build

You will need `Visual Studio 2022 17.1` or later.

* As there's no automatic detection, edit `vs_build_cfg.bat` to match your version of Visual Studio.

* Launch `swag/build/vs_build_llvm.bat`. Note that building LLVM takes a crazy amount of time and memory, and can require multiple tries if you do not have enough ram.

* Launch `swag/build/vs_build_swag.bat`.

If LLVM has been compiled once, you can also use the `Swag.sln` workspace in the `build` subfolder.

## Windows SDK
The Swag runtime contains 3 libraries from the windows SDK, `kernel32.lib`, `ucrt.lib` and `user32.lib`.

You will find them in `bin/runtime/windows-x86-64`.

They are shipped with the compiler to avoid the necessity to install the SDK before building with Swag. That way the compiler can be used "as is".

The version is `10.0.19041.0`

The standard modules (in `bin/std/modules`) can also have dependencies to some other libraries from the SDK (for example `gdi32.lib`). You will find those dependencies in the `publish` folder of the corresponding module.