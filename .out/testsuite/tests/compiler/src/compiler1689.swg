#global skip
func C(value1: const& s32)
{
    v := -value1
    @assert(v == -1)
}

#test
{
    C(1)
}