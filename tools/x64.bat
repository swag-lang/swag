swag test -w:../.out/testsuite --backend:x64 --rebuild %1 %2 %3 %4
swag test -w:../.out/std --backend:x64 --rebuild %1 %2 %3 %4
swag test -w:../.out/reference --backend:x64 --rebuild %1 %2 %3 %4
swag get  -w:../.out/examples --force
swag test -w:../.out/examples --backend:x64 --rebuild %1 %2 %3 %4