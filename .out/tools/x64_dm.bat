swag_devmode test -w:../testsuite --backend:x64 --rebuild %1 %2 %3 %4
swag_devmode test -w:../std --backend:x64 --rebuild %1 %2 %3 %4
swag_devmode test -w:../reference --backend:x64 --rebuild %1 %2 %3 %4
swag_devmode test -w:../examples --backend:x64 --rebuild %1 %2 %3 %4