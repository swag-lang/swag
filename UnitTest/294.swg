#module module294;
using swag;

#unittest error;

//#[printbytecode]
#[test]
func test1: ()
{
	var b = "XYZ";
	@assert(b[4] == 0);
}
