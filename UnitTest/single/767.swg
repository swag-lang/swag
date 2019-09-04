#module module767

struct Point
{
	var x: s32 = 1
	var y: s32
}

impl Point
{
	func opPostCopy(self)
	{
		@print("postcopy!\n")
	}
}

#[swag.printbc]
#test
{	
	var pt1: Point
	var pt2: Point
	pt2 = pt1
}
