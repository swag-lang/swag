


struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opIndex(self, i, j: uint)->s32
	{
		if(i == 0)
			return acast j
		return acast (j * 2)
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