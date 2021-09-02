


struct point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	pt := cast(*point) @alloc(cast(uint) @sizeof(point))
	@free(cast(*u8) pt)
}