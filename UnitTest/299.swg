#module module299;

//#[printbytecode]
#test
{
	@assert(@prop("toto", count) == 4);
	@assert(@prop("totototo", count) == 8);
}
