#global testerror "Err0702"

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