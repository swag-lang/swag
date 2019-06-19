#unittest module module273a;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#import module273b;

	//#[swag.test, swag.printbytecode]
	#[swag.test]
	func main: ()
	{
		module273b.other();
	}
}