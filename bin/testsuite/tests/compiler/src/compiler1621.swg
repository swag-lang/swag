#global testerror "Err0701"

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