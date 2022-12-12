#test
{
    var x: s32
    y := &x
    //var z: ref s32 = y
    z := ref y
    @assert(@typeof(z).name == "ref s32")
    @assert(@typeof(z) == #type ref s32)
    z1 := ref y
    @assert(@typeof(z1).name == "ref s32")
    @assert(@typeof(z1) == #type ref s32)
}