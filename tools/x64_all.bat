call clean.bat
call cross.bat
call x64.bat --cfg:fast-compile
call x64.bat --cfg:debug
call x64.bat --cfg:fast-debug
call x64.bat --cfg:release