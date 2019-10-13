#module module390;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	@assert(@countof({1, 2}) == 2);
}
