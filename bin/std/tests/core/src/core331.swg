using Core

//#[Swag.PrintBc]
#test
{
    let x = Math.map(5'f32, 0, 10, 0, 360)
    @assert(x == 180)
}
