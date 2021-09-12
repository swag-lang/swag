call clean.bat

7z a rel.zip LICENCE
7z a rel.zip LICENCE.md

7z a rel.zip ../bin/*.swg -r
7z a rel.zip ../bin/*.dll -r
7z a rel.zip ../bin/*.lib -r
7z a rel.zip ../bin/*.pdb -r

7z a rel.zip ../bin/swag.exe
7z a rel.zip ../bin/swag.pdb