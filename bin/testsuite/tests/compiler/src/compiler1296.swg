#global testerror "Err0490"

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