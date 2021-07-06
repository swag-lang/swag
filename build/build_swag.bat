rem llvm 12 version du 11/01/2021
call "build_cfg.bat"

%MSBUILD% swag.sln /p:Configuration="Debug" -m -verbosity:m
%MSBUILD% swag.sln /p:Configuration="Release" -m -verbosity:m
%MSBUILD% swag.sln /p:Configuration="Release (DevMode)" -m -verbosity:m
