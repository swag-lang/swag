


struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
impl Point
{
	func(op: string) opAssign(self, v: s32)
	{
		#if(op == "+=")
		{
		}
		#else
		{
			#assert(false)
		}
	}
}

#test
{
	var x : Point
	x += 0
}