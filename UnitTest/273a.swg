#module module273a;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#import module273b;

	//#[swag.test, swag.printbytecode]
	#test
	{
		module273b.other();
	}
}