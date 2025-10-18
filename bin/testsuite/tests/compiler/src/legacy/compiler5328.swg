#global testerror "Err0698"

struct A {}
impl A
{

    #[Swag.Macro]
    mtd(x: bool, y: bool) opVisit01(s: #code void) {}
}
