#module module64;
#unittest pass semantic;

#unittest error;
#run 127's8 + 1's8;
#unittest error;
#run 255'u8 + 1'u8;

#assert 1.0 / 2.0 == 0.5;
