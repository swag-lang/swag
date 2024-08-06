//#global testerror "Err0557"

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