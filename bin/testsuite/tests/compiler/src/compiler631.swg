struct(T) Point
{
	x: T = 10
	y: T = 20
}

#test
{
	var pt: Point'(f32)
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}