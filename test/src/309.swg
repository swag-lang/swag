#module module309;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	var c = "toto";
	@assert(c == "toto");
	@assert(c != "titi");
}
