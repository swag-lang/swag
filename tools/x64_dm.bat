..\bin\swag_devmode test -w:../bin/testsuite --backend:x64 --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/std       --backend:x64 --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/reference --backend:x64 --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/examples  --backend:x64 --rebuild %1 %2 %3 %4