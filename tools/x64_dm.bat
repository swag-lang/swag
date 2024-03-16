..\bin\swag_devmode test -w:../bin/testsuite --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/testsuite --rebuild -ve --silent %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/std       --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/reference --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/examples  --rebuild %1 %2 %3 %4
call scripts_dm.bat --script-run:false %1 %2
call web_dm.bat --output-doc:false %1 %2