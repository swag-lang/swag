#module module297;
#unittest pass semantic;

//#[swag.printbytecode]
func test1: ()-> string
{
	return "toto1";
}

//#[swag.printbytecode]
#test
{
	var c = test1;
	@assert(c == "toto1");
}