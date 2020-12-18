



struct point
{
	x: s32 = 666
}

//#[swag.printbc]
#test
{
	var arr: [2] point
	ptr := &arr[0]
	@assert(ptr[0].x == 666)
}