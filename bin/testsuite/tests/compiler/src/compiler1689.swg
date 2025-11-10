#global skip
func C(value1: const& s32)
{
    var v = -value1
    @assert(v == -1)
}

#test
{
    C(1)
}