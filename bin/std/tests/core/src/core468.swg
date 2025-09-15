using Core, Core.Math

//#[Swag.PrintAsm]
#test
{
    let a, b: Vector2
    let c = a + b
    @assert(c.x == 0)
}
