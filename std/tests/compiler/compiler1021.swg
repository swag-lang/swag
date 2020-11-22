

#test
{
	var ptr : *u8 = autocast @alloc(10)
	ptr = autocast @realloc(ptr, 20)
	loop 20 ptr[@index] = 0
	@free(ptr)
}