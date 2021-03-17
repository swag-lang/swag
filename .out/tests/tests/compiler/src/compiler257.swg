func test1 ()->s32 { return 666; }

#test
{
	@assert(test1() == 666);
}
