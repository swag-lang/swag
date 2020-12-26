;
using swag;

//#[printbc]
#test
{
	var b = "XYZ";
	@assert(b[2] == cast(u8) ("Z"'char));
	@assert(b[3] == 0);
}
