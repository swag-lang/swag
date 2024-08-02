#global testerror "Err0164"
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