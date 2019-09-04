#module module767

var test: s32

struct Point
{
	var x: s32 = 1
	var y: s32
}

impl Point
{
	func opPostCopy(self)
	{
		//@print("postcopy!\n")
		test = 1
	}
}

//#[swag.printbc]
#test
{	
	var pt1: Point
	var pt2: Point
	
	test = 0
	pt2 = pt1
	@assert(test == 1)
}
