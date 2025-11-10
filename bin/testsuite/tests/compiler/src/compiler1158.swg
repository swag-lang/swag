#[Swag.Inline]
func min(x: s32)->s32
{
    return x + x
}

#test
{
    var x = min(1)
}
