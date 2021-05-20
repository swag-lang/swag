using Swag;

//#[printbc]
#test
{
	var b = "abcd";
	@assert(b[0] == cast(u8) ("a"'rune));
	@assert(b[1] == cast(u8) ("b"'rune));
	@assert(b[2] == cast(u8) ("c"'rune));
	@assert(b[3] == cast(u8) ("d"'rune));
}
