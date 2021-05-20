


struct point
{
	x = "toto"
}

var pt: point

//#[Swag.printbc]
#test
{
	@assert(pt.x == "toto")
}