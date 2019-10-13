#module module141;
#unittest pass semantic;

#unittest error;
attr toto(b: bool) -> var;
#[toto]
var a: bool;
