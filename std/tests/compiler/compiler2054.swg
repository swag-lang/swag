#[swag.inline]
func clamp(x: s32)->s32
{
    return x
}

#[swag.inline]
func toto(x: s32)->s32
{
    return clamp(x)
}

//#[swag.printbc]
#test
{
    @assert(toto(6) == 6)
}