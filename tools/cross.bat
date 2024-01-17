..\bin\swag test -w:../bin/testsuite --backend:scbe --rebuild --cfg:debug -m:test_foreign
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:test_call
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:test_foreign
..\bin\swag test -w:../bin/testsuite --backend:scbe --rebuild --cfg:debug -m:test_call

..\bin\swag test -w:../bin/testsuite --backend:scbe --rebuild --cfg:release -m:test_foreign
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:release -m:test_call
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:release -m:test_foreign
..\bin\swag test -w:../bin/testsuite --backend:scbe --rebuild --cfg:release -m:test_call