/*struct Point
{
	x: s32
	y: s32
}

var cpt: s32 = 0

impl Point
{
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

	var pt2: Point
	pt1 = pt2
}
*/