//#global skip
#[Swag.Optim("", false)]
#test
{
    var x: u16 = 100
    y := x %,np 16
    @assert(y == 4)
    @assert(@typeof(y) == u16)
}