#module module273a;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#import module273b;

	//#[swag.test, swag.printbc]
	#test
	{
		module273b.other();
	}
}