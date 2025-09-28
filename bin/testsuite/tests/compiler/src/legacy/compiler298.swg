using Swag;

//#[PrintBc]
func test2 ()-> string
{
	return "toto1";
}

//#[PrintBc]
#test
{
	var res = test2();
	//@op(res, count) == 5;

	//@print(res);
	//@print(test2.count);
}
