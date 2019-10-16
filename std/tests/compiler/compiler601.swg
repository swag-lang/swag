


struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func(op: string) opAssign(self, pt: Point)
	{
		if op == "="
		{
			self.x = pt.x
			self.y = pt.y
		}
		else
		{
			@assert(false)
		}
	}
}

#test
{
	var pt: Point
	var pt1: Point
	pt1.x = 5
	pt1.y = 10
	@assert(pt.y == 0)
	pt = pt1
	@assert(pt.y == 10)
}