#module module917

func sum(nums: ...)
{
	@assert(@countof(nums) == 3)

	@assert(cast(s32) nums[0] == 10)
	@assert(cast(s32) nums[1] == 20)
	@assert(cast(s32) nums[2] == 30)
}

//#[swag.printbc]
func average(nums: ...)
{
    sum(nums)
}

//#[swag.printbc]
#test
{
	average(10, 20, 30)
}