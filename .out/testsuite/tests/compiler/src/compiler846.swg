

//#[Swag.printbc]
#test
{
	var a : *u8 = acast @alloc(10)
	defer @free(a)

	a[0] = 1
	@assert(a[0] == 1)
}