#[Swag.Optimize(.All, false)]
func(T) toto(x, y: T, r: s32)
{
    @assert(x <=> y == r)
}

#test
{
    toto(1's8, 2's8, -1)
    toto(2's8, 1's8, 1)
    toto(1's8, 1's8, 0)

    toto(-2's8, 2's8, -1)
    toto(-1's8, -2's8, 1)
    toto(-1's8, -1's8, 0)

    toto(1'u8, 2'u8, -1)
    toto(2'u8, 1'u8, 1)
    toto(1'u8, 1'u8, 0)

    toto(1's16, 2's16, -1)
    toto(2's16, 1's16, 1)
    toto(1's16, 1's16, 0)

    toto(-2's16, 2's16, -1)
    toto(-1's16, -2's16, 1)
    toto(-1's16, -1's16, 0)

    toto(1'u16, 2'u16, -1)
    toto(2'u16, 1'u16, 1)
    toto(1'u16, 1'u16, 0)
}
