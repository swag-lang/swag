:loop
swag test -w:../swag/tests --backend:x64 --rebuild --devmode %1 %2 %3 %4
rem swag test -w:../swag/std   --backend:x64 --rebuild --devmode %1 %2 %3 %4
goto loop