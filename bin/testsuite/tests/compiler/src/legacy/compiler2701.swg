//#global testerror "Err0119"

#[Swag.Macro]
func toto(stmt: #code void)
{
    #inject(stmt)
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
