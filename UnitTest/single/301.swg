#module module301;

#test
{
	var str = "toto";
	var c = @op(str, count);
	@assert(c == 4);
	
	str = "totototo";
	c = @op(str, count);
	@assert(c == 8);
}
