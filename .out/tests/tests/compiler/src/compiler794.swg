

var cpt: s32 = 0

struct Point
{
}

//#[Swag.printbc]
func(T) toto()
{
	#if @typeof(T) == @typeof(Point)
	{
		cpt |= 1
	}

	#if @typeof(T) != @typeof(Point)
	{
		cpt |= 2
	}
}


//#[Swag.printbc]
#test
{
	cpt = 0
	toto'(Point)()
	@assert(cpt == 1)
	toto'(s32)()
	@assert(cpt == 3)
}