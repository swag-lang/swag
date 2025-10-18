#global testerror "Err0181"
struct A {}
impl A
{

    //#[Swag.Macro]
    /*mtd(x: bool, y: bool) opVisitBytes(s: #code void)
    {
    }*/
}

#test
{
    var x: A
    foreach x
    {
    }
}
