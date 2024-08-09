#global testerror "Err0246"
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
    visit:01 x
    {
    }
}