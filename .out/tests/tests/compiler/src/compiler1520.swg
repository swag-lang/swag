//#[swag.printbc]
#test
{
    a := 0'u32
    @assert(@typeof(a) == @typeof(u32))
    var xx = a
    @assert(@typeof(xx) == @typeof(u32))

    var tt = 0'u32
    @assert(@typeof(tt) == @typeof(u32))
}