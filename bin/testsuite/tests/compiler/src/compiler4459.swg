#global testerror "Err0682"

#[Swag.Macro]
func toto(stmt: #block void)
{
    #macro
    {
        #inject stmt
    }
}

#test
{
    toto()
    {
        @print("x")
    }
}