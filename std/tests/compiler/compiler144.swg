#module "compiler144"
;

#unittest error;
attr toto(b: bool) -> var;
#[toto(0)]
var a: bool;
