struct Point
{
	x: s32
	y: s32
}

var pt: Point = @{1, 2}

#test
{
	@assert(pt.x == 1)
	@assert(pt.y == 2)
}
