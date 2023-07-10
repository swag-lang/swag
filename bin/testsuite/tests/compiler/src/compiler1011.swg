
using Swag

//#[Swag.PrintBc]
#test
{
	let x: any = 6
	let y: s32 = x
	@assert(x == 6)
	@assert(y == 6)
}
