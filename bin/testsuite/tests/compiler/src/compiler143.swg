
;

#global testerror "Err0670";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
