

//#[Swag.PrintBc]
#test
{
	var c = @[1,2,3,4];
	var d: [..] *s32 = @[&c[0]];
	d[0,0] = 666;
	@assert(c[0] == 666);
	d[0,1] = 777;
	@assert(c[0] == 666);
	@assert(c[1] == 777);
}