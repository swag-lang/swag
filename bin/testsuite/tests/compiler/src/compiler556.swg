/*func T()-> {s32, s32}
{
}*/

//#[Swag.PrintBc]
#test
{
	var x: {s32, s32}
	x = @{1, 2}
	@assert(x.item0 == 1)
	@assert(x.item1 == 2)
}