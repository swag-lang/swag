
using Swag

struct Point
{
	x: s32
	y: s32
}

//#[Swag.printbc]
#test
{
	var x : any = Point{10, 20}
	@assert(@kindof(x) == @typeof(Point))

	ptr := cast(*Point) @dataof(x)
	@assert(ptr.x == 10)
	@assert(ptr.y == 20)
}
