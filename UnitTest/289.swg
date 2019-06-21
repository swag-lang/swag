#module module289;
using swag;

#version(FastTest)
{
	#unittest pass backend;
}

#[swag.printbytecode]
#[test]
func test1: ()
{
	var a: s32 = 666;
	var a1: s32 = 500;
	var b = &a;
	var c = b + 1;
	@assert(c[0] == 500);
}
