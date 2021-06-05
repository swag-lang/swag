/*func T()-> {s32, s32}
{
}*/

//#[Swag.printbc]
#test
{
	var x, y: {s32, s32}
	x = @{1, 2}
	y = x
	@assert(y.item0 == 1)
	@assert(y.item1 == 2)
}