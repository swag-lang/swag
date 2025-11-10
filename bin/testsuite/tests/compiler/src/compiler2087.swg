struct(T) BB
{
    alias TYPE = [4] T
    //alias y = {T, T}
}

//#[Swag.PrintBc]
#test
{
    var x: BB's32
    var y: BB's32.TYPE
    @assert(#typeof(y) == #type [4] s32)
    var z: x.TYPE
    @assert(#typeof(z) == #type [4] s32)

    var x1: BB'f32
    @assert(#typeof(x1.TYPE) == #type [4] f32)
}
