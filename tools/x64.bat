..\bin\swag test -w:../bin/testsuite --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/std       --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/reference --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/examples  --rebuild %1 %2 %3 %4

call scripts.bat --script-run:false %1 %2 --ignore-bad-params
call web.bat --output-doc:false %1 %2 --ignore-bad-params