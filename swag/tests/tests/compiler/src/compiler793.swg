var cpt: s32 = 0

struct Point
{
}

//#[swag.printbc]
func(T) toto()
{
	t := @typeof(T)
	@assert(t.name == "__compiler793.Point")
	if(t == @typeof(Point))
		cpt = 1
}


//#[swag.printbc]
#test
{
	cpt = 0
	toto'(Point)()
	@assert(cpt == 1)
}