..\bin\swag doc -w:../bin/std --rebuild
xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.html ..\web\std /Y

..\bin\swag www.swgs
xcopy ..\web\*.* "c:\xampp\htdocs\web\" /E /Y