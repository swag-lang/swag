
;

#global testerror "Err0681";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
