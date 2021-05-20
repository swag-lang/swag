func toto1()->{s32, s32}
{
    return @{0, 1}
}

//#[Swag.printbc]
func toto()->{s32, s32}
{
    result := toto1()
    return @{result.item0, result.item1}
}

#test
{
    var (res1, res2) = toto()
    @assert(res1 == 0)
    @assert(res2 == 1)
}