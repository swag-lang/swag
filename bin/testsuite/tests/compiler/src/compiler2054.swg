#[Swag.Inline]
func clamp(x: s32)->s32
{
    return x
}

#[Swag.Inline]
func toto(x: s32)->s32
{
    return clamp(x)
}

//#[Swag.PrintBc]
#test
{
    @assert(toto(6) == 6)
}