

struct Array
{
	ptr: *Array
	value: s32 = 666
}

//#[Swag.PrintBc]
#test
{
	var arr: Array
	arr.ptr = &arr
	@assert(arr.value == 666)
	@assert(arr.ptr.value == 666)
	arr.value = 10
	@assert(arr.value != 666)
	@assert(arr.ptr.value != 666)
}
