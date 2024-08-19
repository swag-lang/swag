
;

#global testerror "Err0659";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
