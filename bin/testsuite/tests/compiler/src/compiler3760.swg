//#global skip
#[Swag.Optim("", false)]
#test
{
    var x: u8 = 100
    y := x +,np 16
    @assert(y == 116)
    @assert(@typeof(y) == u8)
}