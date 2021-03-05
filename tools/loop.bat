:loop
swag test -w:../swag/tests --backend:x64 --rebuild --devmode --cfg:fast-debug %1 %2 %3 %4
swag test -w:../swag/std   --backend:x64 --rebuild --devmode --cfg:fast-debug %1 %2 %3 %4
goto loop