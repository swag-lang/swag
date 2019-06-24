#module module301;

#test
{
	var str = "toto";
	var c = @prop(str, count);
	@assert(c == 4);
	
	str = "totototo";
	c = @prop(str, count);
	@assert(c == 8);
}
