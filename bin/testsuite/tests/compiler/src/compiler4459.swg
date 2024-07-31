#global testerror "Err0725"

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