#module module918

func sum(x: f32, nums: ...)
{
	@assert(@countof(nums) == 3)

	@assert(cast(s32) nums[0] == 10)
	@assert(cast(s32) nums[1] == 20)
	@assert(cast(s32) nums[2] == 30)
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