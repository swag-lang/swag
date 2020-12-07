


struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	var pt1: Point
	var pt2: Point
	pt2.x = 1
	@assert(pt1 < pt2)
	@assert(pt2 > pt1)
	pt1.x = 10
	@assert(pt1 > pt2)
	@assert(pt2 < pt1)
	
	@assert(pt2 <= pt2)
}

impl Point
{
	func opCmp(self, other: Point)->s32
	{
		if self.x < other.x || self.y < other.y
			return -1
		if self.x > other.x || self.y > other.y
			return 1			
		return 0
	}
}