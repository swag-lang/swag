#module module297;
#unittest pass semantic;

//#[swag.printbytecode]
func test: ()-> string
{
	return "toto1";
}

#unittest error;
//#[swag.printbytecode]
#[test]
func test1: ()
{
	@print(test);
}