#module module292;
using swag;

//#unittest pass backend;
#version(FastTest)
{
	#unittest pass backend;
}

//#[printbc]
#test
{
	var b = "abcd";
	@assert(b[0] == cast(u8) ('a'));
	@assert(b[1] == cast(u8) ('b'));
	@assert(b[2] == cast(u8) ('c'));
	@assert(b[3] == cast(u8) ('d'));
}
