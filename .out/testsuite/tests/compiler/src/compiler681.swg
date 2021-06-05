


struct Point
{
	x: s32 = 100
	y: s32 = 100
	z: s32 = 100
}

//#[Swag.printbc]
#test
{
	var pt : Point{y: 1, z: 666}
	@assert(pt.x == 100)
	@assert(pt.y == 1)
	@assert(pt.z == 666)
}
