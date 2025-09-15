using Core.Math

//#[Swag.PrintBc]
#test
{
    let pt:  Vector2{1, 2}
    let pt1: Vector2 = pt.toNormalized()
    @assert(isEqualEpsilon(pt1.length(), 1))
}
