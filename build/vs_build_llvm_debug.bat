REM Look at the release .bat for instructions

call "vs_build_cfg.bat"

cd "../llvm"
mkdir build
cd build

cmake -G "Visual Studio 17 2022" -DCMAKE_ASM_COMPILER=%MSCL% -DLLVM_ENABLE_PROJECTS=lld ../llvm
%MSBUILD% llvm.sln /p:Configuration=Debug -verbosity:m -m

cd "../../build"
pause