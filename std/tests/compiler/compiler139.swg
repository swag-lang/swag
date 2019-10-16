#module compiler139
#unittest pass semantic;

#unittest error;
attr toto(b: bool) -> var;
#[toto(true)]


