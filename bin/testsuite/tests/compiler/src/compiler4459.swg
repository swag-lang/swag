#global testerror "Err0682"

#[Swag.Macro]
func toto(stmt: #code void)
{
    #macro
    {
        #inject(stmt)
    }
}

#test
{
    toto()
    {
        @print("x")
    }
}