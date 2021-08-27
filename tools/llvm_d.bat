swag_debug test -w:../.out/testsuite --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_debug test -w:../.out/std   --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_debug test -w:../.out/reference --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_debug test -w:../.out/examples --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4