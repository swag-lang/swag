//#[Swag.PrintBc]
#test
{
	var str = "ABCD";
	var p: const *u8 = @dataof(str);
	@assert(p[0] == cast(u8) ("A"'rune));
	@assert(p[1] == cast(u8) ("B"'rune));
	@assert(p[2] == cast(u8) ("C"'rune));
	@assert(p[3] == cast(u8) ("D"'rune));
}
