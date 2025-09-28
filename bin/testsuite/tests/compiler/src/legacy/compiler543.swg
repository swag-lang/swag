


struct point
{
	x: s32 = 10
	y: s32 = 20
}

var pt: point;

#test
{
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}