#global testerror "Err0487"

#[Swag.Mixin]
func toto(x: #code void)
{
    #macro
    {
        #inject(#up x)
    }
}

#test
{
    toto()
}