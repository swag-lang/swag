using swag;

//#[printbc]
#test
{
	var a: s32 = 666;
	var a1: s32 = 500;
	var b = &a;
	var c = b;
	c += 1;
	@assert(c[0] == 500);
	var d = c;
	d -= 1;
	@assert(d[0] == 666);
}
