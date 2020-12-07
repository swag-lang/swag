


struct(T) Point
{
	x: T = 5
	y: T = 10
}

struct(T) Array
{
	pt: Point'T
}

//#[swag.printbc]
#test
{
	var arr: Array'f32
	@assert(arr.pt.x == 5.0)
	@assert(arr.pt.y == 10.0)
}