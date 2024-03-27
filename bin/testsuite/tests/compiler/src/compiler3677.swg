#[Swag.Macro]
func toto(stmt: code)
{
    #mixin stmt
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        struct A {}
        var x: A
        //var x = func(){}
        //x()
    }
}