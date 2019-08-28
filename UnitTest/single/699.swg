#module module699
//#unittest pass syntax

struct Point
{
	var x: s32 = 5
	var y: s32 = 10
}

struct Array
{
	var pt: Point
}

//#[swag.printbc]
#test
{
	var arr: Array
	ptr := &arr.pt
	@assert(ptr.x == 5)
	@assert(ptr.y == 10)
}