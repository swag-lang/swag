


struct point
{
	x: string = "toto"
}

//#[Swag.printbc]
#test
{
	var pt: point
	@assert(pt.x == "toto")
}