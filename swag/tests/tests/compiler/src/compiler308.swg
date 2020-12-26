;
;

var g = "toto";

//#[swag.printbc]
#test
{
	var c = @countof(g);
	@assert(c == 4);
}
