#[Swag.Optimize(.All, false)]
func toto(y: s32)
{
    {
        var x = 2's32
        x >>= y
        @assert(x == 1)
    }
}

#test
{
    toto(1)
}
