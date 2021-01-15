:loop
swag test -w:f:/swag/swag/tests --backend:x64 --rebuild %1 --devmode --num-cores:0
swag test -w:f:/swag/swag/std   --backend:x64 --rebuild %1 --devmode --num-cores:0
goto loop