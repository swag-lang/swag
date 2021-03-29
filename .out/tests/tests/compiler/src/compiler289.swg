//#[swag.printbc]
#test
{
	var a: s32 = 666;
	var a1: s32 = 500;
	var b = &a;
	var c = b + 1;
	@assert(c[0] == 500);
	var d = c - 1;
	@assert(d[0] == 666);
}
