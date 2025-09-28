//#[Swag.PrintBc]
//#global testerror
#test
{
	let ptr1: *void = @alloc(10'u64)
	@free(ptr1)
}