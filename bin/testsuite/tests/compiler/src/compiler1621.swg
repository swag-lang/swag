#global testerror "Err0642"

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