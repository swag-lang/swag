#module module343;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var a: *s32 = null;
	@assert(a == null);
}
