;
;

var g = "toto";

//#[Swag.printbc]
#test
{
	var c = @countof(g);
	@assert(c == 4);
}
