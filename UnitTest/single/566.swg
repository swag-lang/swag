#module module566
//#unittest pass semantic

struct point1
{
	var x: s32
	var y: s32
	var z: s32 = 1
}

//#[swag.printbc]
struct point
{
	var pt: point1
	var pt1: point1
	var pt2: point1
}

//#[swag.printbc]
#test
{
	var pt : point
	@assert(pt.pt.z == 1)
	@assert(pt.pt1.z == 1)
	@assert(pt.pt2.z == 1)
}