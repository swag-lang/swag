#global testerror "Err0670"
struct A {}
impl A
{

    #[Swag.Macro]
    mtd(x: bool, y: bool) opVisitBytes(s: #code void) {}
}

#test
{
    var x: A
    foreach #Byte x
    {
    }
}
