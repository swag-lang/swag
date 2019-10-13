
//#unittest pass semantic

struct point
{
	x = "toto"
}

var pt: point

//#[swag.printbc]
#test
{
	@assert(pt.x == "toto")
}