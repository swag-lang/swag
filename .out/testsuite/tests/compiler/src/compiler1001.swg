using Swag

struct Point
{
	x: s32 = 10
	y: s32 = 20
}

//#[Swag.PrintBc]
#test
{
	var x : any
	var tt: Point

	x = tt
	@assert(@kindof(x) == @typeof(Point))

	ptr := cast(*Point) @dataof(x)
	//@print(ptr.x)
	@assert(ptr.x == 10)
	@assert(ptr.y == 20)
}
