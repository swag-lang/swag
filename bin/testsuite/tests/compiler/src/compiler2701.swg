//#global testerror "Err0501"

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
        toto()
        {

        }
    }
}