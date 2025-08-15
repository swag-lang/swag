..\bin\swag test -w:../bin/testsuite --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/std       --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/reference --rebuild %1 %2 %3 %4
..\bin\swag test -w:../bin/examples  --rebuild %1 %2 %3 %4

call scripts.bat --script-run:false %1 %2 --ignore-bad-params
call examples.bat --ignore-bad-params %1 %2

..\bin\swag test -w:../bin/testsuite --rebuild -ve --silent %1 %2 %3 %4

..\bin\swag test -w:../bin/testsuite --rebuild --force-format %1 %2 %3 %4
..\bin\swag test -w:../bin/std       --rebuild --force-format %1 %2 %3 %4
..\bin\swag test -w:../bin/reference --rebuild --force-format %1 %2 %3 %4
..\bin\swag test -w:../bin/examples  --rebuild --force-format %1 %2 %3 %4