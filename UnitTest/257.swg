#module module257;
#version(FastTest)
{
	#unittest pass backend;
}
#else
{
	func test: ()->s32 { return 666; }

	#[swag.test]
	func main: ()
	{
		@assert(test == 666);
	}
}