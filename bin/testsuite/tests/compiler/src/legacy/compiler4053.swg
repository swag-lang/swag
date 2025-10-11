#test
{
    var x: s32
    let y  = &x
    //var z: &s32 = y
    let z = #ref y
    @assert(#typeof(z).name == "&s32")
    @assert(#typeof(z) == #type &s32)
    let z1 = #ref y
    @assert(#typeof(z1).name == "&s32")
    @assert(#typeof(z1) == #type &s32)
}
