
using Swag

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var x : any
	let tt: s32 = 666//Point(10, 20)

	x = tt
	//@assert(x is Point)

	let ptr = cast(*s32) @dataof(x)
	@assert(dref ptr == 666)
	//@assert(ptr.x == 10)
	//@assert(ptr.y == 20)
}
