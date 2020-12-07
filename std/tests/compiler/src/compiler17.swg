#module "compiler17"
#unittest pass lexer;

145 18446744073709551615

#unittest error;
18446744073709551616

#unittest error;
256'u8
#unittest error;
65536'u16

#unittest error;
200's8
#unittest error;
60_000's16