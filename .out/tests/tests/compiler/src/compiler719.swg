//#[swag.printbc]
//#global testerror
#test
{
	var ptr1: *void = @alloc(10'uint)
	@free(ptr1)
}