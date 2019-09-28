#module module557
#unittest pass semantic

/*func T()-> {s32, s32}
{
}*/

//#[swag.printbc]
#test
{
	var x, y: {s32, s32}
	x = {1, 2}
	y = x
	@assert(y.val0 == 1)
	@assert(y.val1 == 2)
}