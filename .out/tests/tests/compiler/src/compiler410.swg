

//#[Swag.printbc]
#test
{
	//var c = {0};
	//var d: [..] *s32 = {&c[0]};
	//d[0,0] = 1;

	//var c: [..] s32 = {1};
	//@assert(c[0] == 1);
	//c[0] = 10;
	//@assert(c[0] == 10);

	var a = @[0, 1];
	var b = &a;
	var c = b;
	c[1] = 10;
	@assert(a[1] == 10);
}