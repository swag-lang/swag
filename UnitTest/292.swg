#module module292;
using swag;

//#unittest pass backend;
#version(FastTest)
{
	#unittest pass backend;
}

//#[printbytecode]
#[test]
func test1: ()
{
	var b = "toto";
	//@print(b);
}
