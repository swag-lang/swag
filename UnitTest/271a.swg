#module module271a;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#import module271b;

	test
	{
		module271b.other();
	}
}