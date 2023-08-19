..\bin\swag doc -w:../bin/std --rebuild %1
..\bin\swag doc -w:../bin/reference --rebuild %1

REM xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.html ..\www\public\std /Y
REM xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.html ..\web\std /Y

REM ..\bin\swag www.swgs --callstack
REM xcopy ..\www\*.* "d:\program files\xampp\htdocs\www\" /E /Y
REM py ..\web\*.* "d:\program files\xampp\htdocs\web\" /E /Y