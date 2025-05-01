//#global testerror "Err0501"

#[Swag.Macro]
func toto(stmt: #block void)
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