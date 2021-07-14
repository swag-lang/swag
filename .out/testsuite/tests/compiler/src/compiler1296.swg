
#global testerror

#[Swag.Mixin]
func toto(x: code)
{
    #inline
    {
        #mixin x
    }
}

#test
{
    toto()
}