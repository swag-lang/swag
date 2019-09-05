#module module774

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
	var x: s32
	var y: s32
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
	
	@print(cpt)
	@assert(cpt == 2)
}
