#global testerror "Err0671"

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