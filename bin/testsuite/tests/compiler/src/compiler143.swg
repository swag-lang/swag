
;

#global testerror "Err0624";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
