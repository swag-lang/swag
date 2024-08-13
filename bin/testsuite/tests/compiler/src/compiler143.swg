
;

#global testerror "Err0647";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
