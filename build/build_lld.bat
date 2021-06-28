call "build_cfg.bat"

cd "../llvm/lld/build"
cmake -G "Visual Studio 16 2019" ..
%MSBUILD% lld.sln /p:Configuration="Debug" -m -verbosity:m
%MSBUILD% lld.sln /p:Configuration="Release" -m -verbosity:m
xcopy "release\bin\lld-link.exe" "..\..\..\.out" /Y
cd "../../../build"