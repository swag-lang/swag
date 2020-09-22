


struct Point
{
	x: s32 = 100
	y: s32 = 100
	z: s32 = 100
}

//#[swag.printbc]
#test
{
	var pt : Point{x: 0, y: 1, z: 666}
	@assert(pt.x == 0)
	@assert(pt.y == 1)
	@assert(pt.z == 666)
}
