using mthx, core.math

//#[Swag.printbc]
#test
{
	var pt: Vector2{1, 2}
	var pt1: Vector2 = pt.toNormalize()
    @assert(isEqualEpsilon(pt1.length(), 1))
}