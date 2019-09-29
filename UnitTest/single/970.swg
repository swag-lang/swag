#module module970
//#unittest pass semantic

//#[swag.printbc]
#test
{
	var x = [1,2,3]

	@assert(x[0] == 1)
	@assert(x[1] == 2)
	@assert(x[2] == 3)
}
