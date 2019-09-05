#module module702
//#unittest pass syntax

struct(T) Point
{
	x: T = 5
	y: T = 100
}

struct(T) Array
{
	pt: Point!T
}

//#[swag.printbc]
#test
{
	var arr: Array!s32
	@assert(@op(arr, sizeof) == 8)
	ptr := &arr.pt
	@assert(ptr.x == 5)
	@assert(ptr.y == 100)
}