


struct Point
{
	x: s32
	y: s32
	z: s32 = 100
}

//#[Swag.PrintBc]
#test
{
	var pt : Point{1, 666}
	@assert(pt.x == 1)
	@assert(pt.y == 666)
	@assert(pt.z == 100)
}
