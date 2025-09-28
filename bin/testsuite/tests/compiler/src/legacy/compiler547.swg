


struct point
{
	x = "toto"
}

var pt: point

//#[Swag.PrintBc]
#test
{
	@assert(pt.x == "toto")
}