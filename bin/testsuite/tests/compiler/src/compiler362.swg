;
;

//#[Swag.PrintBc]
func test (arr: [2] s32, idx: s32, val: s32)
{
	@assert(arr[idx] == val);
}

//#[Swag.PrintBc]
#test
{
	var arr = @[6, 10];
	test(arr, 0, 6);
	test(arr, 1, 10);

	var arr1 = @[60, 100];
	test(arr1, 0, 60);
	test(arr1, 1, 100);
}
