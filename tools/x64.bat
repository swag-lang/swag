..\bin\swag test -w:../bin/testsuite --backend:x64 --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/std --backend:x64 --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/reference --backend:x64 --rebuild %1 %2 %3 %4
..\bin\swag get  -w:../bin/examples --force
..\bin\swag test -w:../bin/examples --backend:x64 --rebuild %1 %2 %3 %4