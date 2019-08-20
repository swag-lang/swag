#module module606
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

#unittest error
//#[swag.printbc]
impl Point
{
	func(op: string) opAssign(self, v: s32)
	{
		#if(op == "+=")
		{
		}
		#else
		{
			#assert(false, "invalid operator")
		}
	}
}

#test
{
	var x : Point
	x -= 0
}