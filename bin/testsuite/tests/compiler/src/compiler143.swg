
;

#global testerror "Err0665";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
