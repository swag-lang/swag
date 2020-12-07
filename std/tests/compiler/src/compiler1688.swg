func C(value1: const& bool)
{
    @assert(!value1)
}

#test
{
    C(false)
}