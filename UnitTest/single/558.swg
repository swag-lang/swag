#module module558
#unittest pass semantic

struct point
{
	var x: s32
	var y: s32
}

//#[swag.printbytecode]
#test
{
	pt := cast(*point) @alloc(@prop(point, sizeof))
	@free(cast(*u8) pt)
}