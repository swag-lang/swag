

func sum(nums: f64...)
{
	@assert(@countof(nums) == 3)
	@assert(nums[0] == 10)
	@assert(nums[1] == 20)
	@assert(nums[2] == 30)
}

//#[swag.printbc]
func average(nums: f64...)
{
    sum(nums)
}

//#[swag.printbc]
#test
{
	average(10, 20, 30)
}