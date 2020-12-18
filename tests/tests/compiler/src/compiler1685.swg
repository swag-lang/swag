func C(value1, value2: const& s64)
{
    @assert(value1 == value2)
}

#test
{
    C(2's64, 2's64)
}