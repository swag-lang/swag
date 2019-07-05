#module module309;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var c = "toto";
	@assert(c == "toto");
	@assert(c != "titi");
}
