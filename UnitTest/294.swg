#module module294;
using swag;

#unittest error;

#[printbytecode]
#test
{
	var b = "XYZ";
	@assert(b[4] == 0);
}
