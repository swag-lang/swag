..\bin\swag_devmode test -w:../bin/testsuite --backend:x64  --rebuild --cfg:debug -m:test_foreign
..\bin\swag_devmode test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:test_call
..\bin\swag_devmode test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:test_foreign
..\bin\swag_devmode test -w:../bin/testsuite --backend:x64  --rebuild --cfg:debug -m:test_call

call x64_dm.bat --cfg:fast-compile
call x64_dm.bat --cfg:debug
call x64_dm.bat --cfg:fast-debug
call x64_dm.bat --cfg:release
call llvm_dm.bat --cfg:debug

..\bin\swag_devmode test -w:../bin/testsuite --backend:llvm --cfg:fast-debug --rebuild %1 %2 %3 %4