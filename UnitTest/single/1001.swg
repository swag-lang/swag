#module module1001
using swag

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	var x : any 
	var tt: Point
	
	x = tt
	//@assert(x is Point)
	
	ptr := cast(*Point) @dataof(x)
	@print(ptr.x)
	//@assert(ptr.x == 10)
	//@assert(ptr.y == 20)
}
