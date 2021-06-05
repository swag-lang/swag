


struct Point
{
	c: u32 = 5
}

impl Point
{
	func opCount(self)->uint
    {
        return cast(uint) self.c
    }
}

//#[Swag.printbc]
#test
{
	var pt: Point
	cpt := 0
	loop(pt)
	{
		cpt += 1
	}
	@assert(cpt == 5)
}