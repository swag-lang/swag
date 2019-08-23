#module module626
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

impl Point
{
	func(op: string) opIndexAssign(self, v: s32, i: s32)
	{
		#if op == "+="
		{
			if(i == 0)
				self.x += v;
			else
				self.y += v;
		}
	}
}

//#[swag.printbc]
#test
{
	var pt: Point
	pt[1] += 10
	@assert(pt.y == 10)
	pt[1] += 10
	@assert(pt.y == 20)
}