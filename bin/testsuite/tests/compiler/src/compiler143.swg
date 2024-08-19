
;

#global testerror "Err0656";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
