;
#unittest pass semantic;

attr toto(b: bool) -> var;
#[toto(true)]
var a: bool;

attr toto1(b1: bool, b2: bool) -> var;
#[toto1(true, false)]
var aa: bool;