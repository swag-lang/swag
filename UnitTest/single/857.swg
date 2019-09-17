#module module857

struct Point
{
	x: s32
}

impl Point
{
	func opDrop(self)
	{
		@assert(self.x == 1)
		@print("drop")
	}
}
	
#[swag.printbc]
#test
{
	var pt: Point(1)
}