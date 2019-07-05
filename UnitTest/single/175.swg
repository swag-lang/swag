#module module175;
#unittest pass semantic;

#unittest error;
#run '1' / '2';
#unittest error;
#run "1" / "2";
#unittest error;
#run '1' * '2';
#unittest error;
#run true / false;
#unittest error;
#run true + false;
