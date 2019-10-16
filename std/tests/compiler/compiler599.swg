


struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func(op: string) opAssign(self, what: s32)
	{
		if op == "+="
		{
			self.x = what
			self.y = what * 2
		}
	}
}

#test
{
	var pt: Point
	pt += 10
	@assert(pt.x == 10)
	@assert(pt.y == 20)
}