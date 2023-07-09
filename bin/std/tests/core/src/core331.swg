using Core

//#[Swag.PrintBc]
#test
{
    var x = Math.map(5'f32, 0, 10, 0, 360)
    @assert(x == 180)
}