func isLeapYear(year: u16)->s32
{
    let x = year % 3
    return x
}

func isLeapYear1(year: u64)->u64
{
    let x = year % 3
    return x
}

#test
{
    @assert(isLeapYear(100) == 1)
    @assert(isLeapYear(101) == 2)
    @assert(isLeapYear1(100) == 1)
    @assert(isLeapYear1(101) == 2)
}