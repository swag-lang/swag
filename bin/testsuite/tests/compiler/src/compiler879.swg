

//#[Swag.PrintBc]
#test
{
	let t = func(x: s32, y: s32)->s32 => x * y
	@assert(t(2, 4) == 8)
}
