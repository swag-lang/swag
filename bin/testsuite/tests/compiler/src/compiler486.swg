//#[Swag.PrintBc]
#test
{
    let res = T()
    @assert(res.item0 == true)
    @assert(res.item1 == 2)
    @assert(res.item2 == false)
}

//#[Swag.PrintBc]
func T()->{ a: bool, b: s32, c: bool }
{
    return {true, 2, false}
}
