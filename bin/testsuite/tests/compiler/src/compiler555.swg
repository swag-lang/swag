



struct point
{
	x: s32 = 666
}

//#[Swag.PrintBc]
#test
{
	var arr: point
	var ptr = &arr
	@assert(ptr.x == 666)
}