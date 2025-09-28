


struct point
{
	x: string = "toto"
}

//#[Swag.PrintBc]
#test
{
	var pt: point
	@assert(pt.x == "toto")
}