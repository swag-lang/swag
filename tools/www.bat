..\bin\swag doc -w:../bin/std --rebuild
xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.html ..\www\public\std /Y

..\bin\swag www.swgs --callstack
xcopy ..\www\*.* "c:\xampp\htdocs\www\" /E /Y