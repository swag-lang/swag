#module module293;
using swag;

//#unittest pass backend;
#version(FastTest)
{
	#unittest pass backend;
}

//#[printbytecode]
test
{
	var b = "XYZ";
	@assert(b[2] == cast(u8) 'Z');
	@assert(b[3] == 0);
}
