func D(value: s64)
{

}

func C(value1, value2: const& s64)
{
    D(value1)
    @assert(value1 == value2)
}

#test
{
    C(2's64, 2's64)
}