REM llvm 12 version 27/06/2021
REM llvm project must be stored in "swag/llvm" subfolder
REM git clone https://github.com/llvm/llvm-project.git

call "build_cfg.bat"

cd "../llvm/llvm/build"
cmake -G "Visual Studio 16 2019" -DCMAKE_ASM_COMPILER=%MSCL% ..
%MSBUILD% llvm.sln /p:Configuration="Debug" -m -verbosity:m
%MSBUILD% llvm.sln /p:Configuration="Release" -m -verbosity:m
cd "../../../build"
