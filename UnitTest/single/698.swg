#module module698
//#unittest pass syntax

struct Point
{
	var x: s32 = 5
	var y: s32 = 10
}


//#[swag.printbc]
#test
{
	var pt: Point
	ptr := &pt
	@assert(ptr.x == 5)
	@assert(ptr.y == 10)
}