const DaysToMonth365 = @[0'u16, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365]

//#[swag.printbc]
func tcf_toto20(a: u16)
{
    var slice: const [..] u16 = DaysToMonth365
    res1 := slice[a]
    @assert(res1 == 59)
}

#test
{
    tcf_toto20(2)
}