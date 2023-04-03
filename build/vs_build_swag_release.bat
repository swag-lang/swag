call "vs_build_cfg.bat"

%MSBUILD% swag.sln /p:Configuration="Release" -m -verbosity:m
%MSBUILD% swag.sln /p:Configuration="Release (DevMode)" -m -verbosity:m
%MSBUILD% swag.sln /p:Configuration="Release (Stats)" -m -verbosity:m
