/*
struct Point
{
	x: s32
	y: s32
}

var cpt: s32 = 0

impl Point
{
	func opPostCopy(self)
	{
		@assert(false)
	}
	
	func opPostMove(self)
	{
		cpt += 1
	}
	
	//#[swag.printbc]
	func(op: string) opBinary(self, other: Self)->Point
	{
		return Point(self.x + other.x, self.y + other.y) // move
	}
}

//#[swag.printbc]
#test
{	
	cpt = 0
	
	var pt1: Point
	var pt2: Point

	var pt3: Point	
	pt3 = pt1 + pt2 // move
	@assert(cpt == 2)
}
*/