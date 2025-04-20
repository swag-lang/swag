using Core, Core.Math

//#[Swag.PrintAsm]
#test
{
    var a, b: Vector2
    let c = a + b
    @assert(c.x == 0)
}
