#[Swag.inline]
func toto1()->[2] s32
{
    return @[1, 2]
}

//#[Swag.PrintBc]
#test
{
    res := toto1()
    @assert(res[0] == 1)
    @assert(res[1] == 2)
}