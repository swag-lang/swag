#module module287;
#version(FastTest)
{
	#unittest pass backend;
}

//#[swag.printbytecode]
#test
{
	var a: bool = true;
	var b = &a;
	var c = b[0];
	@assert(c == true);
}

#test
{
	var a: s32 = 666;
	var b = &a;
	var c = b[0];
	@assert(c == 666);
}
