#module module547
//#unittest pass semantic

struct point
{
	var x = "toto"
}

var pt: point

//#[swag.printbytecode]
#test
{
	@assert(pt.x == "toto")
}