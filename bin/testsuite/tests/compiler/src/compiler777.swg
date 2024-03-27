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

//#[Swag.PrintBc]
func toto()->Point
{
	var pt: Point
	return pt
}

//#[Swag.PrintBc]
#test
{
	cpt = 0
	toto();
	@assert(cpt == 1)
}

*/