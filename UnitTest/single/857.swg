#module module857

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
	
#[swag.printbc]
#test
{
	{
		var pt: Point(1)
	}
	
	@print(G)
}