#module module258;
#version(FastTest)
{
	#unittest pass backend;
}
#else
{
	func test: (a: s32, b: s32)->s32 
	{ 
		return a - b; 
	}

	#[swag.test]
	func main: ()
	{
		@assert(test(60, 40) == 20);
		@assert(test(50, 50) == 0);
		@assert(test(50, 60) == -10);
	}
}