..\bin\swag_devmode test -w:../bin/testsuite --backend:x64  --rebuild --cfg:debug -m:test_foreign
..\bin\swag_devmode test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:test_call
..\bin\swag_devmode test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:test_foreign
..\bin\swag_devmode test -w:../bin/testsuite --backend:x64  --rebuild --cfg:debug -m:test_call

..\bin\swag_devmode test -w:../bin/testsuite --backend:x64  --rebuild --cfg:release -m:test_foreign
..\bin\swag_devmode test -w:../bin/testsuite --backend:llvm --rebuild --cfg:release -m:test_call
..\bin\swag_devmode test -w:../bin/testsuite --backend:llvm --rebuild --cfg:release -m:test_foreign
..\bin\swag_devmode test -w:../bin/testsuite --backend:x64  --rebuild --cfg:release -m:test_call