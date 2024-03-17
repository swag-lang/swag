..\bin\swag doc -w:../bin/std       %1 %2 %3 %4
..\bin\swag doc -w:../bin/reference %1 %2 %3 %4

xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.php       ..\web /Y
xcopy ..\bin\reference\output\fast-compile-windows-x86_64\*.php ..\web /Y
xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.php       ..\web\html\*.html /Y
xcopy ..\bin\reference\output\fast-compile-windows-x86_64\*.php ..\web\html\*.html /Y

