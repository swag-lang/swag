

//#[Swag.PrintBc]
#test
{
	var a : ^u8 = cast() @alloc(10)
	defer @free(a)

	a[0] = 1
	@assert(a[0] == 1)
}