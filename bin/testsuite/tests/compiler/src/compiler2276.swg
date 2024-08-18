//#[Swag.PrintBc]
#test
{
    var tt: [2] *void
    var ptr = &tt[1]
    ptr[cast(u64),bit -1's64] = ptr
    @assert(tt[0] == ptr)
}