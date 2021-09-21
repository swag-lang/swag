..\bin\swag test -w:../bin/testsuite --backend:x64  --rebuild --cfg:debug -m:tests_foreign
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:tests_call
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:tests_foreign
..\bin\swag test -w:../bin/testsuite --backend:x64  --rebuild --cfg:debug -m:tests_call

call x64.bat --cfg:fast-compile
call x64.bat --cfg:debug
call x64.bat --cfg:fast-debug
call x64.bat --cfg:release
call llvm.bat --cfg:debug

