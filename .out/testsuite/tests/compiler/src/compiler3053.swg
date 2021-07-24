#test
{
    var x: [] typeinfo = @[s32, @typeof([2] s32)]
    @assert(x[0] == s32)
    @assert(x[1] == [2] s32)
}