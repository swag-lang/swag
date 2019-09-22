#module module916

func sum(nums: f64...)
{
	@print(cast(s32) @countof(nums))
}

#[swag.printbc]
func average(nums: f64...)
{
    sum(nums)
}

//#[swag.printbc]
#test
{
	average(10, 20, 30)
}