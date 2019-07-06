#module module390;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	@assert(@prop({1, 2}, count) == 2);
}
