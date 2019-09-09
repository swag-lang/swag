#module module794

var cpt: s32 = 0

struct Point
{
}

//#[swag.printbc]
func(T) toto()
{
	#if @typeof(T) == @typeof(Point)
	{
	}
}


//#[swag.printbc]
#test
{
	cpt = 0
	toto!Point()
	@assert(cpt == 1)
}