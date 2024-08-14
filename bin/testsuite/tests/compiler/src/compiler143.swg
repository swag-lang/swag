
;

#global testerror "Err0648";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
