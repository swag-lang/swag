#module module423
#unittest pass semantic

//#[swag.printbytecode]
#test
{
	let d: s32 = 666;
	let c: const *s32 = &d;
}