


struct point
{
	x: s32
	y: s32
}

var pt: point;

#test
{
	@assert(pt.x == 0)
	@assert(pt.y == 0)
}