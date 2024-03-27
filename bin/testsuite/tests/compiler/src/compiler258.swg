func test1 (a: s32, b: s32)->s32 
{ 
	return a - b; 
}

#test
{
	@assert(test1(60, 40) == 20);
	@assert(test1(50, 50) == 0);
	@assert(test1(50, 60) == -10);
}
