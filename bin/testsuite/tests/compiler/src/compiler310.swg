;
;

var g = false;

//#[Swag.PrintBc]
#test
{
	g = false;
	@assert(g == false);
	g = true;
	@assert(g == true);
	//@print(g);
}
