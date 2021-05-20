;
;

func test(arr: const [..] s32)
{
	@assert(arr[0] == 1);
	@assert(arr[1] == 2);
	@assert(@countof(arr) == 2);
}

//#[Swag.printbc]
#test
{
	test(@[1, 2]);
}
