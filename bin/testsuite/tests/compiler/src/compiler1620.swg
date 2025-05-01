#[Swag.Mixin]
func mac(stmt: #block void)
{
    #inject stmt
}

#test
{
    mac() { return; }
}