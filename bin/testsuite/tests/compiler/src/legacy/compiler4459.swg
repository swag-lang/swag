#global testerror "Err0094"

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
