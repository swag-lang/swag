#module module777

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

//#[swag.printbc]
func toto()->Point
{
	var pt: Point
	return pt
}

//#[swag.printbc]
#test
{	
	toto();
	@assert(cpt == 1)
}