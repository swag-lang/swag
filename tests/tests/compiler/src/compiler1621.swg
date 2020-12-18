
#unittest error

#[swag.mixin]
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