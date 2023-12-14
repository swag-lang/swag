#global testerror "err0557"
struct A {}
impl A {

    //#[Swag.Macro]
    /*mtd(x: bool, y: bool) opVisit,bytes(s: code)
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