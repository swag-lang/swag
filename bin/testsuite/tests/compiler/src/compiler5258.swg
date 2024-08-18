#global testerror "Err0727"
struct A {}
impl A {

    #[Swag.Macro]
    mtd(x: bool, y: bool) opVisitBytes(s: code)
    {
    }
}

#test
{
    var x: A
    visit(Byte) x
    {}
}