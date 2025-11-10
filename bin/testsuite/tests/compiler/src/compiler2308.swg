func toto8(s: string, x: s8...)->s8
{
    var total = 0's8
    foreach c in x do
        total += c
    return total
}

func toto16(s: string, x: s16...)->s16
{
    var total = 0's16
    foreach c in x do
        total += c
    return total
}

func toto32(s: string, x: s32...)->s32
{
    var total = 0's32
    foreach c in x do
        total += c
    return total
}

#test
{
    @assert(toto8("1", 1, 2, 4) == 7)
    @assert(toto16("1", 1, 2, 4) == 7)
    @assert(toto32("1", 1, 2, 4) == 7)
}
