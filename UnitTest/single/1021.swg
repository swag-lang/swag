#module module1021

#test
{
	ptr := @alloc(10)
	ptr = @realloc(ptr, 20)
	@free(ptr)
}