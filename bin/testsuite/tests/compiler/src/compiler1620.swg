#[Swag.Mixin]
func mac(stmt: code)
{
    #mixin stmt
}

#test
{
    mac() { return; }
}