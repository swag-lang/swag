#module module1005
using swag

//#[swag.printbc]
#test
{
	let x: any = 6
	@assert(cast(s32) x == 6)
}
