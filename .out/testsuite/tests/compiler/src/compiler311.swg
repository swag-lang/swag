;
;

var g: string;

//#[Swag.PrintBc]
#test
{
	g = "toto";
	@assert(g == "toto");
	g = "titi";
	@assert(g == "titi");
	//@print(g);
}
