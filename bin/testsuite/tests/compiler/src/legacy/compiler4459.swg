#global testerror "Err0093"

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
