set MSBUILD="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/MSBuild/Current/Bin/msbuild.exe"

cd "../llvm/lld/build"
cmake -G "Visual Studio 16 2019" ..
%MSBUILD% lld.sln /p:Configuration="Debug" -m -verbosity:m
%MSBUILD% lld.sln /p:Configuration="Release" -m -verbosity:m
cd "../../../tools"