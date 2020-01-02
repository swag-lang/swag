//#[swag.printbc]
#test
{
    a := 0'u32
    @assert(@typeof(a) == @typeof(u32))
    let @alias1 = a
    @assert(@typeof(@alias1) == @typeof(u32))
}