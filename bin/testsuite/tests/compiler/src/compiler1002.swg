
using Swag

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	let x : any = Point{10, 20}
	@assert(@kindof(x) == #typeof(Point))

	let ptr = cast(*Point) @dataof(x)
	@assert(ptr.x == 10)
	@assert(ptr.y == 20)
}
