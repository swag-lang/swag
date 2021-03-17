swag clean -w:../examples --cfg:fast-debug --clean-dep --silent
swag test  -w:../examples --cfg:debug      --backend:x64 --rebuild %1 %2 %3 %4
swag test  -w:../examples --cfg:fast-debug --backend:x64 --rebuild %1 %2 %3 %4
swag test  -w:../examples --cfg:release    --backend:x64 --rebuild %1 %2 %3 %4

call x64.bat --cfg-optim-bc:false
call x64.bat --cfg-inline-bc:false
call x64.bat --cfg-safety:false
call x64.bat --cfg:debug
call x64.bat --cfg:fast-debug
call x64.bat --cfg:release

swag test  -w:../examples --cfg:debug --backend:llvm --rebuild %1 %2 %3 %4
call llvm.bat

