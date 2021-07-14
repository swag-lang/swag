struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func(op: string) opUnary(self)->Point
	{
		var pt: Point
		#if op == "-"
		{
			pt.x = -self.x
			pt.y = -self.y
			return pt
		}

		return pt
	}
}

//#[Swag.PrintBc]
#test
{
	var a : Point
	a.x = 1
	a.y = 20
	a = -a
	@assert(a.x == -1)
	@assert(a.y == -20)
}