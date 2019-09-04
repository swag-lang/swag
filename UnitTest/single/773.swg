#module module773

struct Point
{
	var x: s32
	var y: s32
}

var cpt: s32 = 0

impl Point
{
	func opClone(self, other: Self)
	{
		self.x = other.x + 1
		self.y = other.y + 1
	}
	
	func opPostMove(self)
	{
		@assert(self.x == 1)
		cpt += 1
	}
}

//#[swag.printbc]
#test
{	
	var pt1: Point
	pt1	= Point(1, 2)
	@assert(cpt == 1)
	
	var pt2: Point
	pt1 = pt2
}
