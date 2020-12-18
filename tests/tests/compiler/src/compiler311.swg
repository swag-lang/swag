;
;

var g: string;

//#[swag.printbc]
#test
{
	g = "toto";
	@assert(g == "toto");
	g = "titi";
	@assert(g == "titi");
	//@print(g);
}
