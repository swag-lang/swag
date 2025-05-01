#global testerror "Err0487"

#[Swag.Mixin]
func toto(x: #block void)
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