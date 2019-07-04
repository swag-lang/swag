#module module362;
//#unittest pass semantic;

#[swag.printbytecode]
func test: (arr: [2] s32, idx: s32, val: s32)
{
	@assert(arr[idx] == val);
}

#[swag.printbytecode]
#test
{
	var arr = {6, 10};
	test(arr, 0, 6);
	test(arr, 1, 10);
}
