//#global skip
#[Swag.Optim("", false)]
#test
{
    var x: u8 = 100
    y := x /,np 2
    @assert(y == 50)
    @assert(@typeof(y) == u8)
}