swag_debug test -w:../testsuite --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_debug test -w:../std   --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_debug test -w:../reference --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_debug test -w:../examples --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4