

//#[Swag.PrintBc]
#test
{
	t := @(x: s32, y: s32) => x * y
	@assert(t(2, 4) == 8)
}
