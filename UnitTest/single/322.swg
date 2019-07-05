#module module322;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var a: s32 = 666;
	var b = &a;
	var c: *s32;
	c = b;
	@assert(b[0] == 666);
	@assert(c[0] == 666);
	/*@assert(a == 1);
	b[0] = 666;
	@assert(a == 666);*/
}
