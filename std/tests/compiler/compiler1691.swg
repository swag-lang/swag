func C(value1, value2: const& s32)
{
    @assert(value1 + value2 == 5)
    @assert(value2 - value1 == 3)
    @assert(value1 * value2 == value2)
    @assert(value2 / value1 == 4)
    @assert(value1 % value2 == value1)
    @assert(value1 | value2 == 5)
    @assert(value1 & value2 == 0)
}

#test
{
    C(1, 4)
}