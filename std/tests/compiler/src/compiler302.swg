//#[swag.printbc]
#test
{
	var str = "ABCD";
	var p: const *u8 = @dataof(str);
	@assert(p[0] == cast(u8) ("A"'char));
	@assert(p[1] == cast(u8) ("B"'char));
	@assert(p[2] == cast(u8) ("C"'char));
	@assert(p[3] == cast(u8) ("D"'char));
}
