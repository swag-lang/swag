#module module769

struct Point
{
	var x: s32
	var y: s32
}

impl Point
{
	func opAffect(self, other: Self)
	{
		self.x = other.x + 1
		self.y = other.y + 1
	}
}

//#[swag.printbc]
#test
{	
	var pt1: Point
	var pt2: Point(10, 20)
	pt1 = pt2
	@assert(pt1.x == 11)
	@assert(pt1.y == 21)
}
