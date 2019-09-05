#module module558
#unittest pass semantic

struct point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	pt := cast(*point) @alloc(@op(point, sizeof))
	@free(cast(*u8) pt)
}