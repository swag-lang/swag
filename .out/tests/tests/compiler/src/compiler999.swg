
using Swag

//#[Swag.printbc]
#test
{
	var x : any

	x = 6
	@assert(@kindof(x) == s32)
	x = "6"
	@assert(@kindof(x) == string)
	x = 6'f64
	@assert(@kindof(x) == f64)
}
