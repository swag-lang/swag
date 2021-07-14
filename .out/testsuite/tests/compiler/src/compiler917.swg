

func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}

func sum(nums: ...)
{
	@assert(@countof(nums) == 3)
	@assert(valueof's32(nums[0]) == 10)
	@assert(valueof's32(nums[1]) == 20)
	@assert(valueof's32(nums[2]) == 30)
}

//#[Swag.PrintBc]
func average(nums: ...)
{
    sum(nums)
}

//#[Swag.PrintBc]
#test
{
	average(10, 20, 30)
}