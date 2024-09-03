#[Swag.Macro]
func toto(stmt: code)
{
    #inject stmt
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