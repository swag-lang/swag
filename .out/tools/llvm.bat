swag test -w:../testsuite --backend:llvm --rebuild %1 %2 %3 %4
swag test -w:../std --backend:llvm --rebuild %1 %2 %3 %4
swag test -w:../reference --backend:llvm --rebuild %1 %2 %3 %4
swag test -w:../examples --backend:llvm --rebuild %1 %2 %3 %4