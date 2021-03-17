:loop
swag test -w:../tests --backend:x64 --rebuild --devmode %1 %2 %3 %4
swag test -w:../std   --backend:x64 --rebuild --devmode %1 %2 %3 %4
goto loop