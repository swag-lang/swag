//#[swag.printbc]
//#unittest error
#test
{
	var ptr1: *void = @alloc(10'uint)
	@free(ptr1)
}