;
#unittest pass semantic;

#unittest error;
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
