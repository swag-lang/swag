func toto8(s: string, x: s8...)->s8
{
    var total = 0's8
    visit c in x: total += c
    return total
}

func toto16(s: string, x: s16...)->s16
{
    var total = 0's16
    visit c in x: total += c
    return total
}

func toto32(s: string, x: s32...)->s32
{
    var total = 0's32
    visit c in x
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
    var total = 0's64
    visit c in x
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
    @assert(toto8("titi", @spread([1's8, 2, 4])) == 7)
    @assert(toto16("titi", @spread([1's16, 2, 4])) == 7)
    @assert(toto32("titi", @spread([1, 2, 4])) == 7)
    @assert(toto64("titi", @spread([1's64, 2, 4])) == 7)

    var arr = [1, 2, 4]
    @assert(toto32("titi", @spread(arr)) == 7)
    var arr8 = [1's8, 2, 4]
    @assert(toto8("titi", @spread(arr8)) == 7)
}