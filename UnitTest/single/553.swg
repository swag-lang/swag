#module module553
//#unittest pass semantic


struct point
{
	var x: s32 = 666
}
//#[swag.printbytecode]
#test
{
	var arr: [2] point
	@print(arr[0].x)
}