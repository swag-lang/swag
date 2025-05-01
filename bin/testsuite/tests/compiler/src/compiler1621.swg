#global testerror "Err0654"

#[Swag.Mixin]
func mac(stmt: #block void)
{
    #inject stmt
}

#test
{
    mac()
    {
         return 1
    }
}