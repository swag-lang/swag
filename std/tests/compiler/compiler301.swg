//#[swag.printbc]
#test
{
	var str = "toto";
	var c = @countof(str);
	@assert(c == 4);

	str = "totototo";
	c = @countof(str);
	@assert(c == 8);
}
