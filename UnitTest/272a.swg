#unittest module module272a;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#import module272b;

	#[swag.test]
	func main: ()
	{
		module272b.other();
	}
}