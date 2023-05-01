..\bin\swag test -w:../bin/testsuite --backend:llvm --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/std       --backend:llvm --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/reference --backend:llvm --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/examples  --backend:llvm --rebuild %1 %2 %3 %4