#module module344;
//#unittest pass semantic;

#[swag.printbytecode]
#test
{
	var a: s32;
	@assert(a == 0);
}
