#global testerror "Err0718"
struct A {}
impl A {

    //#[Swag.Macro]
    /*mtd(x: bool, y: bool) opVisitBytes(s: code)
    {
    }*/
}

#test
{
    var x: A
    visit x
    {

    }
}