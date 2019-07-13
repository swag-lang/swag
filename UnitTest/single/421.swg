#module module421
#unittest pass semantic

//#[swag.printbytecode]
#unittest error;
#test
{
	let d: s32 = 666;
	d = 20;
}