swag_devmode test -w:../.out/testsuite --backend:x64 --rebuild %1 %2 %3 %4
swag_devmode test -w:../.out/std --backend:x64 --rebuild %1 %2 %3 %4
swag_devmode test -w:../.out/reference --backend:x64 --rebuild %1 %2 %3 %4
swag_devmode get  -w:../.out/examples --force
swag_devmode test -w:../.out/examples --backend:x64 --rebuild %1 %2 %3 %4