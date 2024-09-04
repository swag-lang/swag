#global testerror "Err0476"

#[Swag.Mixin]
func toto(x: code)
{
    #macro
    {
        #inject #up x
    }
}

#test
{
    toto()
}