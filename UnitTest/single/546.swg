#module module546
//#unittest pass semantic

struct point
{
	var x = "toto"
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(pt.x == "toto")
}