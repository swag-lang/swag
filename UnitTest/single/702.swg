#module module702
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 100
}

struct(T) Array
{
	var pt: Point!T
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