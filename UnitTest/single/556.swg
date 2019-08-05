#module module556
#unittest pass semantic

/*func T()-> {s32, s32}
{
}*/

//#[swag.printbytecode]
#test
{
	var x: {s32, s32}
	x = {1, 2}
	@assert(x.0 == 1)
	@assert(x.1 == 2)
}