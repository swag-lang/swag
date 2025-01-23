..\bin\swag_devmode test -w:../bin/testsuite --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/std       --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/reference --rebuild %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/examples  --rebuild %1 %2 %3 %4

call scripts_dm.bat  --ignore-bad-params %1 %2
call examples_dm.bat --ignore-bad-params %1 %2

call web_dm.bat     --output-doc:false --ignore-bad-params %1 %2

..\bin\swag_devmode test -w:../bin/testsuite --rebuild -ve --silent %1 %2 %3 %4

..\bin\swag_devmode test -w:../bin/testsuite --rebuild --force-format %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/std       --rebuild --force-format %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/reference --rebuild --force-format %1 %2 %3 %4
..\bin\swag_devmode test -w:../bin/examples  --rebuild --force-format %1 %2 %3 %4