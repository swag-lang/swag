#global testerror "Err0524"

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