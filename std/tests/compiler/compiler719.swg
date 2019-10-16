//#[swag.printbc]
//#unittest error
#test
{
	var ptr1: *void = @alloc(10'u32)
	@free(ptr1)
}