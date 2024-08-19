#global testerror "Err0716"

#[Swag.Macro]
func toto(stmt: code)
{
    #macro
    {
        #mixin stmt
    }
}

#test
{
    toto()
    {
        @print("x")
    }
}