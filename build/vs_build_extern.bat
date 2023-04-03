call "vs_build_cfg.bat"

%MSBUILD% "..\bin\std\modules\freetype\vendor\freetype-2.10.4\builds\windows\vc2010\freetype.sln" /p:Configuration="Release" -m -verbosity:m
xcopy "..\bin\std\modules\freetype\vendor\freetype-2.10.4\objs\x64\release\freetype.dll" "..\bin\std\modules\freetype\publish\windows-x86_64" /y
xcopy "..\bin\std\modules\freetype\vendor\freetype-2.10.4\objs\x64\release\freetype.lib" "..\bin\std\modules\freetype\publish\windows-x86_64" /y

xcopy %WSDK%\um\x64\dbghelp.lib "..\bin\runtime\windows-x86_64" /y
xcopy %WSDK%\um\x64\kernel32.lib "..\bin\runtime\windows-x86_64" /y
xcopy %WSDK%\um\x64\user32.lib "..\bin\runtime\windows-x86_64" /y
xcopy %WSDK%\ucrt\x64\ucrt.lib "..\bin\runtime\windows-x86_64" /y

xcopy %WSDK%\um\x64\gdi32.lib "..\bin\std\modules\gdi32\publish\windows-x86_64" /y
xcopy %WSDK%\um\x64\dwmapi.lib "..\bin\std\modules\gui\publish\windows-x86_64" /y
xcopy %WSDK%\um\x64\opengl32.lib "..\bin\std\modules\ogl\publish\windows-x86_64" /y
xcopy %WSDK%\um\x64\iphlpapi.lib "..\bin\std\modules\win32\publish\windows-x86_64" /y
xcopy %WSDK%\um\x64\shell32.lib "..\bin\std\modules\win32\publish\windows-x86_64" /y
xcopy %WSDK%\um\x64\xinput.lib "..\bin\std\modules\xinput\publish\windows-x86_64" /y

pause