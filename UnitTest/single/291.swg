#module module291;
using swag;

#version(FastTest)
{
	#unittest pass backend;
}

//#[printbytecode]
func test2 (v: string, v1: string)
{
	//@print(v);
	//@print(v1);
}

//#[printbytecode]
#test
{
	test2("toto", "titi");
}
