//#[Swag.PrintBc]
func isLeapYear(year: s16)->s32
{
    var x = year / 3
    return x
}

func isLeapYear1(year: s64)->u64
{
    var x = year / 3
    return x
}

#test
{
    @assert(isLeapYear(99) == 33)
    @assert(isLeapYear1(99) == 33)
}
