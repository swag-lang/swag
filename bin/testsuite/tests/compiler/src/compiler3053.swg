#test
{
    var x: [?] typeinfo = [s32, #typeof(#type [2] s32)]
    @assert(x[0] == s32)
    @assert(x[1] == #type [2] s32)
}
