#unittest module module266a;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#import module266b;

	#[swag.test]
	func main: ()
	{
	}
}