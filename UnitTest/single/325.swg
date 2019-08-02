#module module325;
#unittest pass semantic;

//#[swag.printbytecode]
#unittest error;
#test
{
	var a: [4] s32;
	a[5] = 1;
}
