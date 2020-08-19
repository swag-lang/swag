#module "compiler175"
;

#unittest error;
#assert '1' / '2';
#unittest error;
#assert "1" / "2";
#unittest error;
#assert '1' * '2';
#unittest error;
#assert true / false;
#unittest error;
#assert true + false;
