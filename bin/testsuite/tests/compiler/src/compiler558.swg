


struct point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	var pt = cast(*point) @alloc(cast(u64) @sizeof(point))
	@free(cast(*u8) pt)
}