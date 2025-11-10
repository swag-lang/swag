#global testerror "Err0556"

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
