;
;

//#[Swag.printbc]
#test
{
	var a: s32 = 1;
	var b = &a;
	b[0] = 666;
	@assert(a == 666);
	@assert(b[0] == a);
}
