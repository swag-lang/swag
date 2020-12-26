using swag;

//#[printbc]
#test
{
	var b = "abcd";
	@assert(b[0] == cast(u8) ("a"'char));
	@assert(b[1] == cast(u8) ("b"'char));
	@assert(b[2] == cast(u8) ("c"'char));
	@assert(b[3] == cast(u8) ("d"'char));
}
