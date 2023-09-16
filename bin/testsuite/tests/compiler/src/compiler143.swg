
;

#global testerror "Err1138";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
