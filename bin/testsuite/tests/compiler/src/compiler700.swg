


struct(T) Point
{
	x: T = 5
	y: T = 100
}

struct Array
{
	pt: Point's32
}

//#[Swag.PrintBc]
#test
{
	var arr: Array
	let ptr = &arr.pt
	@assert(ptr.x == 5)
	@assert(ptr.y == 100)
}