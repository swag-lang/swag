#module module356;
//#unittest pass semantic;

var a = {"0", "1.0"};

//#[swag.printbytecode]
#test
{
	a[1] = "toto";
	@assert(a[1] == "toto");
	a[1] = "1.0";
}
