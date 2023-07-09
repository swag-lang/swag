func isLeapYear(mod: s32)->s32
{
    var x = 100 % mod
    return x
}

func isLeapYear1(mod: s64)->u64
{
    var x = 100 % mod
    return x
}

#test
{
    @assert(isLeapYear(3) == 1)
    @assert(isLeapYear1(3) == 1)
}