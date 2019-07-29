#module module518
#unittest pass semantic

//#[swag.printbytecode]
var x, y = [1,2,3];

#test
{
	@assert(y[0] == 1)
	@assert(y[1] == 2)
	@assert(y[2] == 3)
}
