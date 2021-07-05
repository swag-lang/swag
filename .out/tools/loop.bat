:loop
rem call x64.bat --devmode %1 %2 %3 %4
swag test -w:../testsuite --backend:x64 --rebuild %1 %2 %3 %4 --devmode
goto loop