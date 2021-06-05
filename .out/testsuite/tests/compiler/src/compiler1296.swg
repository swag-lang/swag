
#global testerror

#[Swag.mixin]
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