const DaysToMonth365 = [0'u16, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365]

#test
{
    const slice: const [..] u16 = DaysToMonth365
    const res1   = slice[2]
    @assert(res1 == 59)
}
