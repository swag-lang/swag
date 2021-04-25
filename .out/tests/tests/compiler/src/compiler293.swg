;
using swag;

//#[printbc]
#test
{
	var b = "XYZ";
	@assert(b[2] == cast(u8) ("Z"'rune));
	@assert(b[3] == 0);
}
