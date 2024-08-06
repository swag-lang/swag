#global testerror "Err0491"

struct A {}

impl A
{
    #[Swag.Mixin]
    mtd(a: bool, b: bool) opVisit(st: code) {}    
}