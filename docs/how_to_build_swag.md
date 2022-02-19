# How to build Swag

## Visual Studio

##### You will need `Visual Studio 2022 17.1` or later.

* As there's no automatic detection, edit `vs_build_cfg.bat` to match your version of Visual Studio.
* Launch `swag/build/vs_build_llvm.bat` (this will take some time !).
* Launch `swag/build/vs_build_swag.bat`.

Once LLVM has been compiled once, you can also use the `Swag.sln` workspace in the `build` subfolder.

## LLVM

Swag has two backends, a `x86_64` custom backend written for fast compile, but with far from optimal generated code, and `llvm` for optimized builds.

The LLVM source tree is included in the Swag source tree for convenience. Version is 12.0.1.

https://releases.llvm.org/download.html

## Windows SDK
The Swag runtime contains 3 libraries from the windows SDK, `kernel32.lib`, `ucrt.lib` and `user32.lib`.

You will find them in `bin/runtime/windows-x86-64`.

They are shipped with the compiler to avoid the necessity to install the SDK before building with Swag. That way the compiler can be used "as is".

The version is `10.0.19041.0`

The standard modules (in `bin/std/modules`) can also have dependencies to some other libraries from the SDK (for example `gdi32.lib`). You will find those dependencies in the `publish` folder of the corresponding module.