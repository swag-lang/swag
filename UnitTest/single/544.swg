#module module544
#unittest pass semantic

struct point
{
	var x: s32
	var y: s32
}

var pt: point;

#test
{
	@assert(pt.x == 0)
	@assert(pt.y == 0)
}