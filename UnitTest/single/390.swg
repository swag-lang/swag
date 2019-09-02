#module module390;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	@assert(@op({1, 2}, count) == 2);
}
