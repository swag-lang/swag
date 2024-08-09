#global testerror "Err0692"

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