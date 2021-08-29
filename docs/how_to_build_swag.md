# How to compile Swag
You will need LLVM version 12.0.1 (https://github.com/llvm/llvm-project/releases/tag/llvmorg-12.0.1).
Download the full `Source Code` (zip) at the end of the list.
Unzip that code to a subfolder named `llvm`, at the root of the swag source tree (for example `swag/llvm/`)

## Under Windows
You will need `Visual Studio 2019 16.11.1` or later.

* As there's no automatic detection, edit `build_cfg.bat` to match your version of Visual Studio and the MSVC version (used to find `cl.exe`)
* Launch `build_llvm.bat` (this will take some time !)
* Launch `build_swag.bat`