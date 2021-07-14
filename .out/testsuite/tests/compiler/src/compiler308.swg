;
;

var g = "toto";

//#[Swag.PrintBc]
#test
{
	var c = @countof(g);
	@assert(c == 4);
}
