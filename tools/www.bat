..\bin\swag doc -w:../bin/std       --rebuild %1
..\bin\swag doc -w:../bin/reference --rebuild %1

xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.php               ..\web\doc /Y
xcopy ..\bin\reference\output\fast-compile-windows-x86_64\language.php  ..\web\doc /Y
xcopy ..\bin\reference\output\fast-compile-windows-x86_64\*.php         ..\web\pages /Y
xcopy ..\bin\reference\output\fast-compile-windows-x86_64\index.php     ..\web /Y
xcopy ..\web\*.* "d:\program files\xampp\htdocs\web\" /E /Y
