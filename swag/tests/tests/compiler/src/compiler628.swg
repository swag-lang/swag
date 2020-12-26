


struct Point
{
	x: s32
	y: s32
}

#unittest error
impl Point
{
	func(op: string) opIndexAssign(self, i: f32, v: s32)
	{
		#if op == "="
		{
			if(i == 0)
				self.x = v;
			else
				self.y = v;
		}

	}
}

//#[swag.printbc]
#test
{
	var pt: Point
	pt[0] += 5
	pt[1] += 10
	//@assert(pt.x == 5)
	//@assert(pt.y == 10)
}