..\bin\swag doc -w:../bin/std --rebuild
cd ../bin/examples/scripts
swag langref.swgs
cd ../../../tools

xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.html ..\web\std /Y
