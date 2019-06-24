#module module297;
#unittest pass semantic;

//#[swag.printbytecode]
func test1: ()-> string
{
	return "toto1";
}

#unittest error;
//#[swag.printbytecode]
#test
{
	@print(test1);
}