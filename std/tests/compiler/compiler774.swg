/*
impl Point
{
	func opPostCopy(self)
	{
		@assert(false)
	}
	
	//#[swag.semsleep(100)]
	func opPostMove(self)
	{
		cpt += 1
	}
}

struct Point
{
	x: s32
	y: s32
}

var cpt: s32 = 0

//#[swag.printbc]
func toto()->Point
{
	return Point(1, 2) // one move
}

//#[swag.printbc]
#test
{	
	cpt = 0
	
	var pt: Point
	pt = toto() // one move
	
	//@print(cpt)
	@assert(cpt == 2)
}
*/