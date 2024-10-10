#global testerror "Err0654"

#[Swag.Mixin]
func mac(stmt: code)
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