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
    visit c: x
    {
        //@print(cast(s64)c)
        //@print(" ")
        total += c
    }

    return total
}

func toto64(s: string, x: s64...)->s64
{
    @assert(s == "titi")
    total := 0's64
    visit c: x
    {
        //@print(cast(s64)c)
        //@print(" ")
        total += c
    }

    //@print("\n")
    return total
}

#test
{
    @assert(toto8("titi", @spread(@[1's8, 2, 4])) == 7)
    @assert(toto16("titi", @spread(@[1's16, 2, 4])) == 7)
    @assert(toto32("titi", @spread(@[1, 2, 4])) == 7)
    @assert(toto64("titi", @spread(@[1's64, 2, 4])) == 7)

    arr := @[1, 2, 4]
    @assert(toto32("titi", @spread(arr)) == 7)
    arr8 := @[1's8, 2, 4]
    @assert(toto8("titi", @spread(arr8)) == 7)
}