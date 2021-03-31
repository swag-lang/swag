swag test -w:../tests --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag test -w:../std --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4
swag test -w:../howto --backend:llvm ---cfg:debug --rebuild %1 %2 %3 %4
swag get  -w:../examples
swag test -w:../examples --backend:llvm --cfg:debug --rebuild %1 %2 %3 %4