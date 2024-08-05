#global testerror "Err0536"

struct A {}

impl A
{
    #[Swag.Mixin]
    mtd(a: bool, b: bool) opVisit(st: code) {}    
}