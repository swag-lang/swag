#[swag.inline]
func toto(x: s32)->s32
{
    return x + x
}

//#[swag.printbc]
#test
{
    r := toto(1)
    r = toto(1)
}
