#module module308;
#unittest pass semantic;

var g = "toto";

//#[swag.printbytecode]
#test
{
	var c = @prop(g, count);
	@assert(c == 4);
}
