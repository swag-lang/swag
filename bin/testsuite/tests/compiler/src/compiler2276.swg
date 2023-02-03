//#[Swag.PrintBc]
#test
{
    var tt: [2] *void
    ptr := &tt[1]
    ptr[cast,bit(u64) -1's64] = ptr
    @assert(tt[0] == ptr)
}