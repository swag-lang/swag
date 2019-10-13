#module module556
#unittest pass semantic

/*func T()-> {s32, s32}
{
}*/

//#[swag.printbc]
#test
{
	var x: {s32, s32}
	x = {1, 2}
	@assert(x.val0 == 1)
	@assert(x.val1 == 2)
}