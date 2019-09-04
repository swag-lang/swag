#module module770

struct Point
{
	var x: s32
	var y: s32
}

var cpt: s32 = 0

impl Point
{
	func opPostMove(self)
	{
		cpt += 1
	}
}

#[swag.printbc]
#test
{	
	var pt1: Point
	pt1	= Point(1, 2)
	@assert(cpt == 1)
}
