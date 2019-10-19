func test2 (v: s32)->s32
{
	if(v == 1)
		return 1;
	return v * test1(v - 1);
}

//#[swag.printbc]
func test1 (v: s32)->s32
{
	if(v == 1)
		return 1;
	return v * test2(v - 1);
}

#test
{
	@assert(test2(5) == 120);
	@assert(test2(4) == 24);
}
