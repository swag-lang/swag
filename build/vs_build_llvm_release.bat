REM Be sure the specify the correct path to visual studio in "vs_build_cfg.bat"
REM To compile LLVM, you NEED to install cmake and python !!!

REM LLVM version 15.0.7 (https://github.com/llvm/llvm-project/releases/tag/llvmorg-15.0.7)
REM LLVM downloaded file is "llvm-project-15.0.7.src.tar.xz"
REM LLVM project must be stored in "swag/llvm" subfolder

call "vs_build_cfg.bat"

cd "../llvm"
mkdir build
cd build

cmake -G "Visual Studio 17 2022" -DCMAKE_ASM_COMPILER=%MSCL% -DLLVM_ENABLE_PROJECTS=lld ../llvm
%MSBUILD% llvm.sln /p:Configuration="Release" -m -verbosity:m

cd "../../build"
pause