#[swag.mixin]
func toto(x: s32)
{
}

#test
{
    x := 0
    toto(x)
}