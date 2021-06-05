using Swag

//#[printbc]
#test
{
    var x: const* TypeInfo
    x = s32
    @assert(x == s32)
    @assert(x == @typeof(s32))

    var x1: const* TypeInfo
    var x2: const* TypeInfo
    x1 = s32
    x2 = s32
    @assert(x1 == x2)
}
