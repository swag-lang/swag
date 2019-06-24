#module module298;
#version(FastTest)
{
	#unittest pass backend;
}
#else
{
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
}