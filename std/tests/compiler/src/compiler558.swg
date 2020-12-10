


struct point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	pt := cast(*point) @alloc(cast(uint) @sizeof(point))
	@free(cast(*u8) pt)
}