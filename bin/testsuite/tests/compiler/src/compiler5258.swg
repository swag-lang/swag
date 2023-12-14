#global testerror "err1139"
struct A {}
impl A {

    #[Swag.Macro]
    mtd(x: bool, y: bool) opVisit,bytes(s: code)
    {
    }
}

#test
{
    var x: A
    visit,Bytes x
    {}
}