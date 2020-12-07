#module "compiler729"

struct Array
{
	values: [] s32 = @[1, 2, 3]
}

//#[swag.printbc]
#unittest error
#test
{
	var arr: Array
	@assert(arr.values[0] == 1)
	@assert(arr.values[1] == 2)
	@assert(arr.values[2] == 3)
	@assert(arr.values[3] == 3)
}
