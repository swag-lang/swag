call clean.bat
call cross_dm.bat
call x64_dm.bat --cfg:fast-compile
call x64_dm.bat --cfg:debug
call x64_dm.bat --cfg:fast-debug
call x64_dm.bat --cfg:release