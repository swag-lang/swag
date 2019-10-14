

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{	
	var pt1: Point
	pt1 = 1
	@assert(pt1.x == 1)
	@assert(pt1.y == 2)
}

impl Point
{
	func opAffect(self, other: s32)
	{
		self.x = other
		self.y = other + 1
	}
}
