struct(T) BB
{
    alias TYPE = [4] T
    //alias y = {T, T}
}

//#[Swag.printbc]
#test
{
    var x: BB's32
    var y: BB's32.TYPE
    @assert(@typeof(y) == [4] s32)
    var z: x.TYPE
    @assert(@typeof(z) == [4] s32)

    var x1: BB'f32
    @assert(@typeof(x1.TYPE) == [4] f32)
}