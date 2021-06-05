
using Swag

struct Point
{
	x: s32
	y: s32
}

//#[Swag.printbc]
#test
{
	var x : any
	var tt: s32 = 666//Point(10, 20)

	x = tt
	//@assert(x is Point)

	ptr := cast(*s32) @dataof(x)
	@assert(ptr[0] == 666)
	//@assert(ptr.x == 10)
	//@assert(ptr.y == 20)
}
