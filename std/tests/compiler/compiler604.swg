


struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func(op: string) opAssign(self, pt: s32)
	{
	}
}

#test
{
	x := 4
	x *= 5
	@assert(x == 20)
	x = 3
	x %= 3
	@assert(x == 0)
}