#module module312;
#import module312b;

var g: string = "toto";
var g1: string = "titi";

//#[swag.printbytecode]
#test
{
	@assert(g == "toto");
	@assert(g1 == "titi");
}
