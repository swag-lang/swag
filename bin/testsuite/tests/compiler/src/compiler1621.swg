#global testerror "Err0673"

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