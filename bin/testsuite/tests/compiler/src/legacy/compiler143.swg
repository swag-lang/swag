
;

#global testerror "Err0495";
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
