#global testerror "Err0429"

struct A {}

impl A
{
    #[Swag.Mixin]
    mtd(a: bool, b: bool) opVisit(st: code) {}    
}