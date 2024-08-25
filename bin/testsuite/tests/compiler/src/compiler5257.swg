#global testerror "Err0277"
/*struct A {}
impl A {

    #[Swag.Macro]
    mtd(x: bool, y: bool) opVisit01(s: code)
    {
    }
}*/

#test
{
    var x: A
    foreach<01> x
    {
    }
}