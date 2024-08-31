#global testerror "Err0524"

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