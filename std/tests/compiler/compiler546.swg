


struct point
{
	x: string = "toto"
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(pt.x == "toto")
}