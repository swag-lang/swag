var cpt: s32 = 0

struct Point
{
}

//#[Swag.PrintBc]
func(T) toto()
{
	t := @typeof(T)
	@assert(t.name == "__compiler793.Point")
	if(t == @typeof(Point))
		cpt = 1
}


//#[Swag.PrintBc]
#test
{
	cpt = 0
	toto'(Point)()
	@assert(cpt == 1)
}