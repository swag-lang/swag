using swag;

//#[printbc]
func test2 ()-> string
{
	return "toto1";
}

//#[printbc]
#test
{
	var res = test2();
	//@op(res, count) == 5;
		
	//@print(res);
	//@print(test2.count);
}
