#[Swag.Mixin]
func mac(stmt: #code void)
{
    #inject(stmt)
}

#test
{
    mac()
    {
        return
    }
}
