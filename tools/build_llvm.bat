rem llvm 12 version du 11/01/2021
call "build_cfg.bat"

cd "f:/swag/llvm/llvm/build"
cmake -G "Visual Studio 16 2019" -DCMAKE_ASM_COMPILER=%MSCL% ..
%MSBUILD% llvm.sln /p:Configuration="Debug" -m -verbosity:m
%MSBUILD% llvm.sln /p:Configuration="Release" -m -verbosity:m
cd "../../../tools"
