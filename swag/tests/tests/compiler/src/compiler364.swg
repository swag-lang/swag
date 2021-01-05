#global testerror;

//#[swag.printbc]
func test (arr: [2] s32, idx: s32, val: s32)
{
	@assert(arr[idx] == val);
}

//#[swag.printbc]
#test
{
	var arr = @{6, 10, 10};
	test(arr, 0, 6);
}
