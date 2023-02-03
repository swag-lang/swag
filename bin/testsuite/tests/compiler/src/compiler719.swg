//#[Swag.PrintBc]
//#global testerror
#test
{
	var ptr1: *void = @alloc(10'u64)
	@free(ptr1)
}