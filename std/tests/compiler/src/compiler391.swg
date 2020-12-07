;
;

func test (arr: const [2] s32)
{
	@assert(arr[0] == 1);
	@assert(arr[1] == 2);
}

//#[swag.printbc]
#test
{
	test(@[1, 2]);
}
