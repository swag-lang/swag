# How to build Swag

## LLVM
##### You will need LLVM version 12.0.1 (https://github.com/llvm/llvm-project/releases/tag/llvmorg-12.0.1)
And everything necessary to build it (like `cmake`)...

Download the full `Source Code` (zip) at the end of the list.
Unzip that code to a subfolder named `llvm`, at the root of the swag source tree (for example `swag/llvm/`)

## Windows 10
##### You will need the `Windows Sdk 10.0.19041.0` (https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/).

This is also necessary to build an executable with the Swag compiler.

## Visual Studio

##### You will need `Visual Studio 2019 16.11.1` or later.

As there's no automatic detection, edit `build_cfg.bat` to match your version of Visual Studio.
Launch `swag/build/build_llvm.bat` (this will take some time !).
Launch `swag/build/build_swag.bat`.