#module module299;

//#[printbc]
#test
{
	@assert(@prop("toto", count) == 4);
	@assert(@prop("totototo", count) == 8);
}
