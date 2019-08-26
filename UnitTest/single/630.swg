#module module630
//#unittest pass semantic

struct(T) Point
{
	var x: T
	var y: T
}

//#[swag.printbc]
#test
{
	var pt: Point!(f32)
	@assert(pt.x == 0)
	@assert(pt.y == 0)
}