call clean.bat
call cross_dm.bat

call clean.bat
call x64_dm.bat --cfg:release
call x64_dm.bat --cfg:fast-compile
call x64_dm.bat --cfg:fast-debug
call x64_dm.bat --cfg:debug

call clean.bat
call llvm_dm.bat --cfg:release
call llvm_dm.bat --cfg:fast-compile
call llvm_dm.bat --cfg:fast-debug
call llvm_dm.bat --cfg:debug

call examples_dm.bat --bytecode --cfg:fast-debug
call examples_dm.bat --cfg:fast-debug
call examples_dm.bat --cfg:release --backend:llvm

call scripts_dm.bat --cfg:release
call scripts_dm.bat --cfg:fast-compile
call scripts_dm.bat --cfg:debug
call scripts_dm.bat --cfg:fast-debug
