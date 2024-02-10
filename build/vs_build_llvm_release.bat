REM Be sure the specify the correct path to visual studio in "vs_build_cfg.bat"
REM To compile LLVM, you NEED to install cmake and python !!!
REM https://cmake.org/download/
REM https://www.python.org/downloads/

REM LLVM version 17.0.6 (https://github.com/llvm/llvm-project/releases/tag/llvmorg-17.0.6)
REM LLVM downloaded file is "llvm-project-17.0.6.src.tar.xz"
REM LLVM project must be stored in "swag/llvm" subfolder

REM Activating "-m" option with MSBUILD is not possible for me (takes too much memory). 
REM You can add it for faster compile if you have enough RAM and CPU power (> 16 go).

call "vs_build_cfg.bat"

cd "../llvm"
mkdir build
cd build

cmake -G "Visual Studio 17 2022" -DCMAKE_ASM_COMPILER=%MSCL% -DLLVM_ENABLE_PROJECTS=lld ../llvm
%MSBUILD% llvm.sln /p:Configuration=Release -verbosity:m

cd "../../build"
pause