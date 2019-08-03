#module module552
//#unittest pass semantic


//#[swag.printbytecode]
#test
{
	a := @alloc(10'u32 * 4)
	@free(a)
}