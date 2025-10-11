//#[Swag.PrintBc]
#test
{
    let res = T(0)
    @assert(res.item0 == 1)
    @assert(res.item1 == 2)
    @assert(res.item2 == 3)
}

//#[Swag.PrintBc]
func T(a: s32)->{ a, b, c: s32 }
{
    return {1, 2, 3}
}
