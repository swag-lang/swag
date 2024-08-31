#global testerror "Err0702"

#[Swag.Mixin]
func mac(stmt: code)
{
    #mixin stmt
}

#test
{
    mac()
    {
         return 1
    }
}