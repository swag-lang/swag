#module "compiler606"

struct Point
{
	x: s32
	y: s32
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