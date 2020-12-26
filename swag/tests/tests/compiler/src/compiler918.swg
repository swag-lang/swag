

func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}

func sum(x: f32, nums: ...)
{
	@assert(@countof(nums) == 3)

	@assert(valueof's32(nums[0]) == 10)
	@assert(valueof's32(nums[1]) == 20)
	@assert(valueof's32(nums[2]) == 30)
}

//#[swag.printbc]
func average(nums: ...)
{
    sum(1, nums)
}

//#[swag.printbc]
#test
{
	average(10, 20, 30)
}