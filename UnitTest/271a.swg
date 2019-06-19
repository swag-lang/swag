#unittest module module271a;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#import module271b;

	#[swag.test]
	func main: ()
	{
		module271b.other();
	}
}