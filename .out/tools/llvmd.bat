swag_d test -w:../tests --backend:llvm --rebuild %1 %2 %3 %4
swag_d test -w:../std   --backend:llvm --rebuild %1 %2 %3 %4
swag_d test -w:../howto --backend:llvm --rebuild %1 %2 %3 %4
swag_d get  -w:../examples
swag_d test -w:../examples --backend:llvm --rebuild %1 %2 %3 %4