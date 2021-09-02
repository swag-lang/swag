//#[Swag.PrintBc]
#test
{
	var c = 666;
	var d: [..] s32 = @[1, c, 2];

	@assert(d[0] == 1);
	@assert(d[2] == 2);
	@assert(d[1] == 666);
}
