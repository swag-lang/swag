/*
struct Point
{
	x: s32
	y: s32
}

var cpt: s32 = 0

impl Point
{
	/*func opAffect(self, other: Self)
	{
		self.x = other.x + 1
		self.y = other.y + 1
	}*/

	func opPostMove(self)
	{
		@assert(self.x == 1)
		cpt += 1
	}
}

//#[Swag.printbc]
#test
{
	cpt = 0
	var pt1: Point
	pt1	= Point(1, 2)
	@assert(cpt == 1)
}
*/