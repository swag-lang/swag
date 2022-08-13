..\bin\swag test -w:../bin/testsuite --backend:x64  --rebuild --cfg:debug -m:test_foreign
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:test_call
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:test_foreign
..\bin\swag test -w:../bin/testsuite --backend:x64  --rebuild --cfg:debug -m:test_call

call x64.bat --cfg:fast-compile
call x64.bat --cfg:debug
call x64.bat --cfg:fast-debug
call x64.bat --cfg:release
call llvm.bat --cfg:debug

..\bin\swag test -w:../bin/testsuite --backend:llvm --cfg:fast-debug --rebuild --cfg-optim-speed:false %1 %2 %3 %4
..\bin\swag test -w:../bin/testsuite --backend:llvm --cfg:fast-debug --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/std --backend:llvm --cfg:fast-debug --rebuild %1 %2 %3 %4
call scripts.bat
