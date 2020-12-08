func toto8(s: string, x: s8...)->s8
{
    total := 0's8
    visit c: x total += c
    return total
}

func toto16(s: string, x: s16...)->s16
{
    total := 0's16
    visit c: x total += c
    return total
}

func toto32(s: string, x: s32...)->s32
{
    total := 0's32
    visit c: x total += c
    return total
}

#test
{
    @assert(toto8("1", 1, 2, 4) == 7)
    @assert(toto16("1", 1, 2, 4) == 7)
    @assert(toto32("1", 1, 2, 4) == 7)
}