#module module680
//#unittest pass syntax

struct Point
{
	var x: s32
	var y: s32
	var z: s32 = 100
}

//#[swag.printbc]
#test
{
	var pt : Point(1, 666)
	@assert(pt.x == 1)
	@assert(pt.y == 666)
	@assert(pt.z == 100)
}
