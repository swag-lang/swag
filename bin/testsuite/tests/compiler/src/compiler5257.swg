#global testerror "Err0686"
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
    foreach #01 x // swc-expected-error {{parser_err_invalid_compiler}}
    {
    }
}