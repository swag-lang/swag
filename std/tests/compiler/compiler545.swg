


struct point1
{
	x: s32 = 10
	y: s32 = 20
}

struct point
{
	pt: point1
	x: s32 = 30
	y: s32 = 40
}

var pt: point;

#test
{
	@assert(pt.pt.x == 10)
	@assert(pt.pt.y == 20)
	@assert(pt.y == 40)
	@assert(pt.x == 30)
}