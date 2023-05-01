call clean.bat
call cross_dm.bat
call llvm_dm.bat --cfg:fast-compile
call llvm_dm.bat --cfg:debug
call llvm_dm.bat --cfg:fast-debug
call llvm_dm.bat --cfg:release