;
;

//#[Swag.printbc]
#test
{
	const a = 666;
	var slice = @[1, a, 2];
	@assert(slice[1] == 666);
}
