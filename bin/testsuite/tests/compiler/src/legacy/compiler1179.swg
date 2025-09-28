#[Swag.Inline]
func toto(x: s32)->s32
{
    return x + x
}

//#[Swag.PrintBc]
#test
{
    var r = toto(1)
    r = toto(1)
}
