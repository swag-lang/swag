//#[Swag.PrintBc]
#test
{
    let res  = T()
    let res1 = res
    @assert(res1.item0 == true)
    @assert(res1.item1 == 2)
    @assert(res1.item2 == false)
}

//#[Swag.PrintBc]
func T()->{ a: bool, b: s64, c: bool }
{
    return {true, 2, false}
}
