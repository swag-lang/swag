


struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opIndex(self, i: u64)->s32
	{
		if(i == 0):
			return self.x
		return self.y
	}
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	pt.x = 10
	pt.y = 20
	@assert(pt[0] == 10)
	@assert(pt[1] == 20)
}