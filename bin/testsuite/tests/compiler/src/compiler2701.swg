//#global testerror "Err0502"

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