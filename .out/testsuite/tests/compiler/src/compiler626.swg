struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func(op: string) opIndexAssign(self, i: uint, v: s32)
	{
		#if op == "+="
		{
			if(i == 0)
				self.x += v;
			else
				self.y += v;
		}
		#if op == "-="
		{
			if(i == 0)
				self.x -= v;
			else
				self.y -= v;
		}
	}
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	pt[1] += 10
	@assert(pt.y == 10)
	pt[1] += 10
	@assert(pt.y == 20)
	pt[1] -= 5
	@assert(pt.y == 15)
}