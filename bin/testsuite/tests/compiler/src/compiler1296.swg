#global testerror "Err0528"

#[Swag.Mixin]
func toto(x: code)
{
    #macro
    {
        #mixin #up x
    }
}

#test
{
    toto()
}