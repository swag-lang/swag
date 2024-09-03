#global testerror "Err0733"

#[Swag.Macro]
func toto(stmt: code)
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