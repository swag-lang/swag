#module module768

var test: s32

struct Point
{
	var x: s32 = 1
	var y: s32
}

struct Point1
{
	var x: s32 = 1
	var y: s32
	var pt: Point
	var pt1: Point
}

impl Point
{
	func opPostCopy(self)
	{
		//@print("postcopy!\n")
		test += 1
		self.x = 2
		self.y = 4
	}
}

//#[swag.printbc]
#test
{	
	var pt1: Point1
	var pt2: Point1
	
	test = 0
	pt2 = pt1
	@assert(test == 2)
	@assert(pt2.pt.x == 2)
	@assert(pt2.pt.y == 4)
	@assert(pt2.pt1.x == 2)
	@assert(pt2.pt1.y == 4)	
}
