#module module543
#unittest pass semantic

struct point
{
	var x: s32 = 10
	var y: s32 = 20
}

var pt: point;

#test
{
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}