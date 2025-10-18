#global testerror "Err0724"

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
