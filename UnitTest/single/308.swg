#module module308;
#unittest pass semantic;

var g = "toto";

//#[swag.printbc]
#test
{
	var c = @op(g, count);
	@assert(c == 4);
}
