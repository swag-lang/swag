#module module971
//#unittest pass semantic

struct Point
{
	x: s32 = 1
	y: s32 = 2
}

var x: Point// = {1,2,3}

//#[swag.printbc]
#test
{
	@assert(x.x == 1)
	@assert(x.y == 2)
	//@assert(x.val2 == 3)
}
