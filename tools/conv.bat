..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:tests_foreign
..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild --cfg:debug -m:tests_call --test-filter:call5

..\bin\swag test -w:../bin/testsuite --backend:x64 --rebuild  --cfg:debug -m:tests_foreign
