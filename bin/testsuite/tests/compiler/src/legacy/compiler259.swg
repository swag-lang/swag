func test1 (a: f32, b: f32)->f32 
{ 
	return a - b; 
}

#test
{
	@assert(test1(60.0, 40.0) == 20);
	@assert(test1(50.0, 50.0) == 0);
	@assert(test1(50.0, 60.0) == -10);
}
