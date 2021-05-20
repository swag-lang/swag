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
	}

	func opPostMove(self)
	{
		cpt += 1
	}
}

//#[Swag.printbc]
func toto()->Point
{
	var pt: Point
	return pt
}

//#[Swag.printbc]
#test
{
	cpt = 0
	toto();
	@assert(cpt == 1)
}

*/