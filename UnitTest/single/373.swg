#module module373;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	var slice: [..] u8 = "titititi";
	@assert(@op(slice, count) == 8);
	slice = "toto";
	@assert(@op(slice, count) == 4);
	@assert(cast(char) slice[0] == 't');
	@assert(cast(char) slice[1] == 'o');
}
