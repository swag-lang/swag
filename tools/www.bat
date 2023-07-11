..\bin\swag doc -w:../bin/std --rebuild %1
xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.html ..\www\public\std /Y
xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.html ..\web\std /Y

..\bin\swag www.swgs --callstack
xcopy ..\www\*.* "d:\program files\xampp\htdocs\www\" /E /Y
xcopy ..\web\*.* "d:\program files\xampp\htdocs\web\" /E /Y