#module module257;
#version(FastTest)
{
	#unittest pass backend;
}
#else
{
	func test1: ()->s32 { return 666; }

	test
	{
		@assert(test1 == 666);
	}
}