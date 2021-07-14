using Core.Math

//#[Swag.PrintBc]
#test
{
	var pt: Vector2{5, 100}
	pt.normalize()
    @assert(isEqualEpsilon(pt.length(), 1))
}