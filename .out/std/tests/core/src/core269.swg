using Core.Math

//#[Swag.printbc]
#test
{
	var pt: Vector2{1, 2}
	pt1 := pt.toNormalize()
    @assert(isEqualEpsilon(pt1.length(), 1))
}