#module "compiler422"


//#[swag.printbc]
#unittest error
#test
{
	let d: s32 = 666;
	let c: *s32 = &d;
}