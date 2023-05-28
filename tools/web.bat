..\bin\swag doc -w:../bin/std
swag langref.swgs
xcopy ..\bin\std\output\fast-compile-windows-x86_64\*.html ..\web\std /Y

for %%f in (../web/md/*.md) do (
    pandoc "../web/md/%%f" -o "../web/%%~nf.php" --template=../web/md/template.html
)

xcopy ..\web\*.* "D:\Program Files\xampp\htdocs\web" /E /Y