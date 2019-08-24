#module module629
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
	pt.x = 10
	@assert(pt.x == 10.0)
}