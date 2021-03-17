#global testerror

#[swag.macro]
func toto(stmt: code)
{
    #mixin stmt
}

//#[swag.printbc]
#test
{
    toto()
    {
        toto()
        {

        }
    }
}