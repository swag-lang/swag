#global skip
func C(value: const& s32)
{
    var v = cast(u32) value
    @assert(v == 1)
}

#test
{
    C(1)
}