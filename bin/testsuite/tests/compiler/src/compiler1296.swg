#global testerror "Err0485"

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