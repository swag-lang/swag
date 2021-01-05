
#global testerror

#[swag.mixin]
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