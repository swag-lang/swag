call clean.bat
call cross.bat
call llvm.bat --cfg:fast-compile
call llvm.bat --cfg:debug
call llvm.bat --cfg:fast-debug
call llvm.bat --cfg:release