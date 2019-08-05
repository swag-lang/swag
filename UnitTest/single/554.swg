#module module554
#unittest pass semantic


struct point
{
	var x: s32 = 666
}

#[swag.printbytecode]
#test
{
	var arr: [2] point
	ptr := &arr[0]
	if(ptr[0].x == 666){}
}