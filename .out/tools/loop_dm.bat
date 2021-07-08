:loop
rem test -w:../testsuite --backend:x64 --rebuild %1 %2 %3 %4 --file-filter:3018
call x64_dm.bat
goto loop