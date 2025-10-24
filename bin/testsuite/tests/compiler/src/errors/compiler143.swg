
;

#global testerror "Err0223";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
