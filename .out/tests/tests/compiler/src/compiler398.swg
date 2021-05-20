func test1(a: const [3] s32)
{
	@assert(a[0] == 1);
	@assert(a[1] == 666);
	@assert(a[2] == 2);
}

//#[Swag.printbc]
#test
{
	c := 666;
	test1(@[1, c, 2]);
}
