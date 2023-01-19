#[Swag.PrintBc]
func toto(y: s32)->s32
{
    x := y
    x |= 6
    return x
}

#test
{
    y := toto(1)
    @assert(y == 7)
}