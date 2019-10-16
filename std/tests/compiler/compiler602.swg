


struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func(op: string) opAssign(self, pt: Point)
	{
		if op == "<<="
		{
			self.x = pt.x
			self.y = pt.y
		}
		else if op == ">>="
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
	pt <<= pt1
	pt1 >>= pt
}