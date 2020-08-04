#test
{
    var x: [2] s32
    @assert(@kindof(x[0]).name == "s32")
}