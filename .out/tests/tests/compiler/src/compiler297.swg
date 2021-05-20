//#[Swag.printbc]
func test1 ()-> string
{
	return "toto1";
}

//#[Swag.printbc]
#test
{
	var c = test1();
	@assert(c == "toto1");
}