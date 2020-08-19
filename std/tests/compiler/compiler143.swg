#module "compiler143"
;

#unittest error;
attr toto(b: bool) -> var;
#[toto(true, true)]
var a: bool;
