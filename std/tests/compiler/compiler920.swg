#module "compiler920"

func sum(x: f32, nums: f32...)
{
}

//#[swag.printbc]
#unittest error
func average(nums: f32...)
{
    sum(1, nums, 1)
}
