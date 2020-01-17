


struct Point
{
	c: u32 = 5
}

impl Point
{
	func opCount(self)->u32 
    { 
        return self.c
    }
}

//#[swag.printbc]
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