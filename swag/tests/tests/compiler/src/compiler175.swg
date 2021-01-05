
;

#global testerror;
#assert '1' / '2';
#global testerror;
#assert "1" / "2";
#global testerror;
#assert '1' * '2';
#global testerror;
#assert true / false;
#global testerror;
#assert true + false;
