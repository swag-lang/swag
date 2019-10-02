#module module999
using swag

//#[swag.printbc]
#test
{
	var x : any 
	
	x = 6
	@assert(x is s32)
	x = "6"
	@assert(x is string)
	x = 6'f64
	@assert(x is f64)
}
