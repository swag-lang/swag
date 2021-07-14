struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func(op: string) opBinary(self, other: Point)->Point
	{
		var pt: Point
		#if op == "+"
		{
			pt.x = self.x + other.x
			pt.y = self.y + other.y
			return pt
		}

		#if op == "-"
		{
			pt.x = self.x - other.x
			pt.y = self.y - other.y
			return pt
		}

		return pt
	}
}

//#[Swag.PrintBc]
#test
{
	var a,b,c : Point
	a.x = 1
	b.x = 10;
	c = a + b
	@assert(c.x == 11)
	c = a - b
	@assert(c.x == -9)
}