


struct(T) Point
{
	x: T = 6
}

//#[Swag.PrintBc]
#test
{
	var pt: Point's32
	//pt.x = 10
	//pt.opInit()
	@assert(pt.x == 6)
}
