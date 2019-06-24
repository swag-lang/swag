#module module298;
#unittest pass semantic;
using swag;

#[printbytecode]
func test2: ()-> string
{
	return "toto1";
}

#[printbytecode]
#[test]
func test1: ()
{
	@print(test2);
}