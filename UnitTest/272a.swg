#module module272a;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#import module272b;

	test
	{
		module272b.other();
	}
}