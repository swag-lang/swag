#[Swag.Mixin]
func mac(stmt: code)
{
    #inject stmt
}

#test
{
    mac() { return; }
}