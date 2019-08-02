#module module545
//#unittest pass semantic

struct point1
{
	var x: s32 = 10
	var y: s32 = 20
}

struct point
{
	var pt: point1
	var x: s32 = 30
	var y: s32 = 40
}

var pt: point;

#test
{
	@assert(pt.pt.x == 10)
	@assert(pt.pt.y == 20)
	@assert(pt.x == 30)
	@assert(pt.y == 40)
}