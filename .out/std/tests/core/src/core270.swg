using Core.Math

//#[Swag.PrintBc]
#test
{
	var pt: Vector2{1, 2}
    @assert(!pt.isZeroEpsilon(0.01))
    @assert(!pt.isZeroEpsilon())
}