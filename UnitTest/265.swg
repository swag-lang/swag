#module module265;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	func test: (v: s32)->s32
	{
		if(v == 1)
			return 1;
		return v * test1(v - 1);
	}

	//#[swag.printbytecode]
	func test1: (v: s32)->s32
	{
		if(v == 1)
			return 1;
		return v * test(v - 1);
	}

	#[swag.test, swag.public]
	func main: ()
	{
		@assert(test(5) == 120);
		@assert(test(4) == 24);
	}
}