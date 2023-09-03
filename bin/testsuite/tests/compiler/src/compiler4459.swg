#global testerror "Err0122"

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