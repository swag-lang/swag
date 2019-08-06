#module module565
//#unittest pass semantic

struct point1
{
	var x: s32
	var y: s32
	var z: s32
}

//#[swag.printbc]
struct point
{
	var pt: point1
	var x: s32
	var y: s32
	var z: s32
}

#test
{
	var pt : point
	@assert(pt.pt.x == 0)
	pt.pt.x = 10
	@print(pt.pt.x)
	@assert(pt.pt.x == 10)
	pt.pt.opInit()
	@assert(pt.pt.x == 0)
}