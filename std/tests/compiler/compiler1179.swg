#[swag.inline]
func toto(x, y: s32 = 666)
{
    z := x + y
}

//#[swag.printbc]
#test
{
    toto()
    toto()
}
