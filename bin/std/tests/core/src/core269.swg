using Core.Math

//#[Swag.PrintBc]
#test
{
    var pt: Vector2{1, 2}
    var pt1 = pt.toNormalize()
    @assert(isEqualEpsilon(pt1.length(), 1))
}
