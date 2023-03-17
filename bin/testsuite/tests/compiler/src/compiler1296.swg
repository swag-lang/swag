#global testerror

#[Swag.Mixin]
func toto(x: code)
{
    #macro
    {
        #mixin x
    }
}

#test
{
    toto()
}