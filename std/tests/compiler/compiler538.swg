


struct point1
{
	x: s32 = 666
	y: s32
	z: s32 = 100
}

struct point
{
	in: point1
	x: s32 = 10
	y: s32
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(pt.x == 10)
	@assert(pt.in.x == 666)
	@assert(pt.in.z == 100)
}