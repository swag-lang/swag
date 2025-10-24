#global testerror "Err0659"

#[Swag.Mixin]
func mac(stmt: #code void)
{
    #inject(stmt)
}

#test
{
    mac()
    {
        return 1
    }
}
