call "vs_build_cfg.bat"

%MSBUILD% swag.sln /p:Configuration=Release -m -verbosity:m
%MSBUILD% swag.sln /p:Configuration=ReleaseDevMode -m -verbosity:m
%MSBUILD% swag.sln /p:Configuration=ReleaseStats -m -verbosity:m
