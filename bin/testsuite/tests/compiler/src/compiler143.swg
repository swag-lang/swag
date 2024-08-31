
;

#global testerror "Err0672";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
