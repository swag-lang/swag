//#[swag.printbc]
#test
{
	var str = "ABCD";
	var p: const *u8 = @dataof(str);
	@assert(p[0] == cast(u8) ('A'));
	@assert(p[1] == cast(u8) ('B'));
	@assert(p[2] == cast(u8) ('C'));
	@assert(p[3] == cast(u8) ('D'));
}
