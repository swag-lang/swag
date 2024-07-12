REM Be sure the specify the correct path to visual studio in "vs_build_cfg.bat"
REM To compile LLVM, you NEED to install cmake and python !!!
REM https://cmake.org/download/
REM https://www.python.org/downloads/

REM LLVM version 17.0.6 (https://github.com/llvm/llvm-project/releases/tag/llvmorg-17.0.6)
REM LLVM downloaded file is "llvm-project-17.0.6.src.tar.xz"
REM LLVM project must be stored in "swag/llvm" subfolder

call "vs_build_cfg.bat"

cd "../llvm"
mkdir build
cd build

cmake -G "Visual Studio 17 2022" -DCMAKE_ASM_COMPILER=%MSCL% -DLLVM_ENABLE_PROJECTS=lld ../llvm
%MSBUILD% llvm.sln /p:Configuration=Release -verbosity:m -m

cd "../../build"
pause