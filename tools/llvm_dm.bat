swag_devmode test -w:../.out/testsuite --backend:llvm --rebuild %1 %2 %3 %4
swag_devmode test -w:../.out/std --backend:llvm --rebuild %1 %2 %3 %4
swag_devmode test -w:../.out/reference --backend:llvm --rebuild %1 %2 %3 %4
swag_devmode test -w:../.out/examples --backend:llvm --rebuild %1 %2 %3 %4