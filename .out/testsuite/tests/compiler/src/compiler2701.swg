#global testerror

#[Swag.macro]
func toto(stmt: code)
{
    #mixin stmt
}

//#[Swag.printbc]
#test
{
    toto()
    {
        toto()
        {

        }
    }
}