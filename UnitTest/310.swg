#module module310;
//#unittest pass semantic;

var g = false;

//#[swag.printbytecode]
#test
{
	g = false;
	@assert(g == false);
	g = true;
	@assert(g == true);
	//@print(g);
}
