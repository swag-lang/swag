swag_d test -w:../tests --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_d test -w:../std   --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_d test -w:../reference --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag_d test -w:../examples --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4