REM llvm version 12.0.1 (https://github.com/llvm/llvm-project/releases/tag/llvmorg-12.0.1)
REM llvm project must be stored in "swag/llvm" subfolder
REM You NEED to install cmake and python !!!

call "vs_build_cfg.bat"

cd "../llvm"
mkdir build
cd build

cmake -G "Visual Studio 17 2022" -DCMAKE_ASM_COMPILER=%MSCL% -DLLVM_ENABLE_PROJECTS=lld ../llvm
%MSBUILD% llvm.sln /p:Configuration="Debug" -m -verbosity:m
%MSBUILD% llvm.sln /p:Configuration="Release" -m -verbosity:m

cd "../../build"
