
;

#global testerror "Err0669";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
