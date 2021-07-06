swag_devmode test -w:../testsuite --backend:llvm --rebuild %1 %2 %3 %4
swag_devmode test -w:../std --backend:llvm --rebuild %1 %2 %3 %4
swag_devmode test -w:../reference --backend:llvm --rebuild %1 %2 %3 %4
swag_devmode test -w:../examples --backend:llvm --rebuild %1 %2 %3 %4