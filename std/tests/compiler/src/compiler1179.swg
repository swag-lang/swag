#[swag.inline]
func toto(x: s32)->s32
{
    return x + x
}

//#[swag.printbc]
#test
{
    toto(1)
    toto(1)
}
