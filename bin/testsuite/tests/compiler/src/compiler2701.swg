//#global testerror "Err0490"

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