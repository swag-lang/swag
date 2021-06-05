


struct(T) Point
{
	x: T = 5
	y: T = 100
}

struct(T) Array
{
	pt: Point'T
}

//#[Swag.printbc]
#test
{
	var arr: Array's32
	@assert(@sizeof(arr) == 8)
	//ptr := &arr.pt
	//@print(ptr.x)
	//@assert(ptr.y == 100)
}