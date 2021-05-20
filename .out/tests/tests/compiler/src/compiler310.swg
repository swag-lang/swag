;
;

var g = false;

//#[Swag.printbc]
#test
{
	g = false;
	@assert(g == false);
	g = true;
	@assert(g == true);
	//@print(g);
}
