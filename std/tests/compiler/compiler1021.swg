

#test
{
	var ptr : *u8 = acast @alloc(10)
	ptr = acast @realloc(ptr, 20)
	loop 20 ptr[@index] = 0
	@free(ptr)
}