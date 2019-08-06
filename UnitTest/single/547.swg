#module module547
//#unittest pass semantic

struct point
{
	var x = "toto"
}

var pt: point

//#[swag.printbc]
#test
{
	@assert(pt.x == "toto")
}