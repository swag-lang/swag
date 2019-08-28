#module module689
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 10
}

//#[swag.printbc]
#test
{
	var pt : Point!u64(100'u64, 500'u64)
	@assert(pt.x == 100)
	@assert(pt.y == 500)
}