//#[Swag.PrintBc]
#test
{
    let res = T()
    @assert(res.item0 == 1)
    @assert(res.item1 == 2)
    @assert(res.item2 == 3)
}

//#[Swag.PrintBc]
func T()->{ a, b, c: s64 }
{
    return {1's64, 2's64, 3's64}
}
