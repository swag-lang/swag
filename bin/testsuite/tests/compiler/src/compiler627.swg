struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opIndexAffect(self, i: uint, v: s32)
	{
        if(i == 0)
            self.x = v;
        else
            self.y = v;
	}
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	pt[0] = 5
	pt[1] = 10
	@assert(pt.x == 5)
	@assert(pt.y == 10)
}