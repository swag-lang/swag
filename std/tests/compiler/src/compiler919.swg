#module "compiler919"

func sum(x: f32, nums: ...)
{
}

//#[swag.printbc]
#unittest error
func average(nums: ...)
{
    sum(1, nums, 1)
}
