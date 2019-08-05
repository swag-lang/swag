#module module555
//#unittest pass semantic


struct point
{
	var x: s32 = 666
}

//#[swag.printbytecode]
#test
{
	var arr: point
	ptr := &arr
	@assert(ptr.x == 666)
}