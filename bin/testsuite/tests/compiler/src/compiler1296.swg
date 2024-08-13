#global testerror "Err0483"

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