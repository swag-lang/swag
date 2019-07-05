#module module326;
//#unittest pass semantic;

//#[swag.printbytecode]
#unittest error;
#test
{
	var a: [4] s32;
	var c = a[5];
}
