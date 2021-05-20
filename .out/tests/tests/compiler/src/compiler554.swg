



struct point
{
	x: s32 = 666
}

//#[Swag.printbc]
#test
{
	var arr: [2] point
	ptr := &arr[0]
	@assert(ptr[0].x == 666)
}