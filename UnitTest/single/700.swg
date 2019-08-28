#module module700
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 100
}

struct Array
{
	var pt: Point!s32
}

//#[swag.printbc]
#test
{
	var arr: Array
	ptr := &arr.pt
	@assert(ptr.x == 5)
	@assert(ptr.y == 100)
}