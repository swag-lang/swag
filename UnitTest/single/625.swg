#module module625
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

impl Point
{
	func opIndex(self, i: s32, j: s32)->s32
	{
		if(i == 0)
			return j
		return j * 2
	}
}

//#[swag.printbc]
#test
{
	var pt: Point
	pt.x = 10
	pt.y = 20
	@assert(pt[0, 100] == 100)
	@assert(pt[1, 100] == 200)
}