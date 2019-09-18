#module module858

var G: s32

struct Point
{
	x: s32
}

impl Point
{
	func opDrop(self)
	{
		@assert(self.x == 1)
		G += 1
	}
}
	
//#[swag.printbc]
#test
{
	loop(10)
	{
		var pt: Point(1)
	}
	
	//@print(G)
	@assert(G == 10)
}