call "vs_build_cfg.bat"

%MSBUILD% "..\bin\std\modules\freetype\vendor\freetype-2.10.4\builds\windows\vc2010\freetype.sln" /p:Configuration=Release /p:Platform=x64 -m -verbosity:m
xcopy "..\bin\std\modules\freetype\vendor\freetype-2.10.4\objs\x64\release\freetype.dll" "..\bin\std\modules\freetype\publish\windows-x86_64" /y
xcopy "..\bin\std\modules\freetype\vendor\freetype-2.10.4\objs\x64\release\freetype.lib" "..\bin\std\modules\freetype\publish\windows-x86_64" /y

pause