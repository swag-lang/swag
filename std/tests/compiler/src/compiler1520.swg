//#[swag.printbc]
#test
{
    a := 0'u32
    @assert(@typeof(a) == @typeof(u32))
    var @alias1 = a
    @assert(@typeof(@alias1) == @typeof(u32))

    var tt = 0'u32
    @assert(@typeof(tt) == @typeof(u32))
}