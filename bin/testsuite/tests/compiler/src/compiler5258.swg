#global testerror "Err0658"
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
    foreach<Byte> x
    {}
}