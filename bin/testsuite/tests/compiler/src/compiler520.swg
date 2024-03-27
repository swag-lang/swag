


struct point
{
	x, y: f32
}

//#[Swag.PrintBc]
#test
{
	var pt: point
	@assert(@sizeof(pt) == 8)
}
