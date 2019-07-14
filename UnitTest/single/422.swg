#module module422
#unittest pass semantic

//#[swag.printbytecode]
#unittest error
#test
{
	let d: s32 = 666;
	let c: *s32 = &d;
}