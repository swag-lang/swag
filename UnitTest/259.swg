#module module259;
#version(FastTest)
{
	#unittest pass backend;
}
#else
{
	func test: (a: f32, b: f32)->f32 
	{ 
		return a - b; 
	}

	#[swag.test]
	func main: ()
	{
		@assert(test(60.0, 40.0) == 20);
		@assert(test(50.0, 50.0) == 0);
		@assert(test(50.0, 60.0) == -10);
	}
}