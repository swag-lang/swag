#module module557
#unittest pass semantic

/*func T()-> {s32, s32}
{
}*/

//#[swag.printbytecode]
#test
{
	var x, y: {s32, s32}
	x = {1, 2}
	y = x
	@assert(y.0 == 1)
	@assert(y.1 == 2)
}