#module module298;
#version(FastTest)
{
	#unittest pass backend;
}
#else
{
	using swag;

	//#[printbytecode]
	func test2: ()-> string
	{
		return "toto1";
	}

	//#[printbytecode]
	#test
	{
		var res = test2;
		//@print(res);
		//@print(test2.count);
	}
}