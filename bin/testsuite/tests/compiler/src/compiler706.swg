


struct Point
{
	c: u32 = 5
}

impl Point
{
	func opCount(self)->u64
    {
        return cast(u64) self.c
    }
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	var cpt = 0
	loop pt
	{
		cpt += 1
	}
	@assert(cpt == 5)
}