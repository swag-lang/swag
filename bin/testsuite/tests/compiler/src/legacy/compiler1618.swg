func toto1()->{ a, b: s32 }
{
    return {0, 1}
}

//#[Swag.PrintBc]
func toto()->{ a, b: s32 }
{
    let result = toto1()
    return {result.item0, result.item1}
}

#test
{
    let (res1, res2) = toto()
    @assert(res1 == 0)
    @assert(res2 == 1)
}
