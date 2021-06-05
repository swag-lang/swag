

var cpt: u32 = 0

struct Point
{
}

//#[Swag.printbc]
func(T) toto()
{
	t := @typeof(T)
	if(t == @typeof(s32))
		cpt |= 1
	if(t == @typeof(f32))
		cpt |= 2
	//if(t == @typeof(Point))
	//	cpt |= 4
}


//#[Swag.printbc]
#test
{
	cpt = 0
	toto's32()
	toto'f32()
	//toto!Point()
	@assert(cpt == 3)
}