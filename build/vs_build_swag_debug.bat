call "vs_build_cfg.bat"

%MSBUILD% swag.sln /p:Configuration=Debug -m -verbosity:m
