using Core.Math

//#[Swag.PrintBc]
#test
{
    var pt:  Vector2{1, 2}
    var pt1: Vector2 = pt.toNormalized()
    @assert(isEqualEpsilon(pt1.length(), 1))
}
