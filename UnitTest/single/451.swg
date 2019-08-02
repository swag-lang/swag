#module module451
#unittest pass semantic

//#[swag.printbytecode]
#unittest error
#test
{
	var a: *s32 = null;
	a[0] = 0
}