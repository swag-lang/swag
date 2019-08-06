#module module553
//#unittest pass semantic


struct point
{
	var x: s32 = 666
	var y: s32 = 500
}

//#[swag.printbc]
#test
{
	var arr: [2] point
	@assert(arr[0].x == 666)
	@assert(arr[0].y == 500)
}