#module module392;
//#unittest pass semantic;

func test(arr: const [..] s32)
{
	@assert(arr[0] == 1);
	@assert(arr[1] == 2);
	@assert(@prop(arr, count) == 2);
}

//#[swag.printbytecode]
#test
{
	test({1, 2});
}
