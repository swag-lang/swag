//#global skip
#[Swag.Optim("", false)]
#test
{
    var x: u16 = 1975
    var y: u16 = 8
    z := x -,np y /,np 10
    @assert(z == 1975)
    @assert(@typeof(z) == u16)
}