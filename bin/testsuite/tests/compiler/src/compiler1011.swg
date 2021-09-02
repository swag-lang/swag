
using Swag

//#[Swag.PrintBc]
#test
{
	var x: any = 6
	var y: s32 = x
	@assert(x == 6)
	@assert(y == 6)
}
