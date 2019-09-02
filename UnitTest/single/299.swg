#module module299;

//#[printbc]
#test
{
	@assert(@op("toto", count) == 4);
	@assert(@op("totototo", count) == 8);
}
