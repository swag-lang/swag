# How to build Swag

## Visual Studio

##### You will need `Visual Studio 2022 17.1` or later.

* As there's no automatic detection, edit `vs_build_cfg.bat` to match your version of Visual Studio.
* Launch `swag/build/vs_build_llvm.bat` (this will take some time !).
* Launch `swag/build/vs_build_swag.bat`.

You can also use the `Swag.sln` workspace in the `build` subfolder (does not contain `llvm`)

## LLVM

Swag has two backends, a `x86_64` custom backend written for fast compile, but with far from optimal generated code, and `llvm` for optimized builds.

The LLVM source tree is included in the Swag source tree for convenience.
https://releases.llvm.org/download.html
Version is 12.0.1
