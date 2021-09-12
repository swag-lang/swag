..\bin\swag_debug test -w:../bin/testsuite --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
..\bin\swag_debug test -w:../bin/std   --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
..\bin\swag_debug test -w:../bin/reference --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
..\bin\swag_debug test -w:../bin/examples --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4