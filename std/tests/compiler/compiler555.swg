



struct point
{
	x: s32 = 666
}

//#[swag.printbc]
#test
{
	var arr: point
	ptr := &arr
	@assert(ptr.x == 666)
}