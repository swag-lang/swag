#global testerror "Err0487"

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