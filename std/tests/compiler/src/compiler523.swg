


struct point
{
	x: s32
	y: s32
	z: s32
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(pt.x == 0)
	@assert(pt.y == 0)
	@assert(pt.z == 0)
}
