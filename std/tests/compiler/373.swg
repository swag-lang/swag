;
//#unittest pass semantic;

//#[swag.printbc]
#test
{
	var slice: [..] u8 = "titititi";
	@assert(@countof(slice) == 8);
	slice = "toto";
	@assert(@countof(slice) == 4);
	@assert(cast(char) slice[0] == 't');
	@assert(cast(char) slice[1] == 'o');
}
