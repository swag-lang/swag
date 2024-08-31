#global testerror "Err0733"

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