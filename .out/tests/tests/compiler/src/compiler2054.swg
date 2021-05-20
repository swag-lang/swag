#[Swag.inline]
func clamp(x: s32)->s32
{
    return x
}

#[Swag.inline]
func toto(x: s32)->s32
{
    return clamp(x)
}

//#[Swag.printbc]
#test
{
    @assert(toto(6) == 6)
}