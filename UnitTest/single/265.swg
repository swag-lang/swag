#module module265;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	func test2 (v: s32)->s32
	{
		if(v == 1)
			return 1;
		return v * test1(v - 1);
	}

	//#[swag.printbytecode]
	func test1 (v: s32)->s32
	{
		if(v == 1)
			return 1;
		return v * test2(v - 1);
	}

	#[swag.public]
	#test
	{
		@assert(test2(5) == 120);
		@assert(test2(4) == 24);
	}
}