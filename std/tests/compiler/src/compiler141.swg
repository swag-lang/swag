#module "compiler141"
;

#unittest error;
attr toto(b: bool) -> var;
#[toto]
var a: bool;
