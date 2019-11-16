struct point
{
	x: s32 = 666
	y: s32 = 500
}

//#[swag.printbc]
#test
{
	var arr: [2] point
	@assert(arr[0].x == 666)
	@assert(arr[0].y == 500)
}