//#global testerror "Err0605"

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
        toto()
        {
        }
    }
}
