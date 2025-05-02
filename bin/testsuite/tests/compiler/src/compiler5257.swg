#global testerror "Err0134"
/*struct A {}
impl A {

    #[Swag.Macro]
    mtd(x: bool, y: bool) opVisit01(s: #code void)
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