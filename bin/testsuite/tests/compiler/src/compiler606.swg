#global testerror "invalid operator"

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
impl Point
{
	func(op: string) opAssign(me, v: s32)
	{
		#if(op == "+=")
		{
		}
		#else
		{
			#error("invalid operator")
		}
	}
}

#test
{
	var x : Point
	x -= 0
}