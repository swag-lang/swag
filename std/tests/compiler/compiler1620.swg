#[swag.mixin]
func mac(stmt: code)->s32
{
    #mixin stmt
}

#test
{
    mac() { return 1; }
}