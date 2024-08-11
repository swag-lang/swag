//#global testerror "Err0497"

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