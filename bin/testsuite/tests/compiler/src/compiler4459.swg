#global testerror "Err0714"

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