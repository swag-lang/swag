#module module390;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	@assert(@prop({1, 2}, count) == 2);
}
