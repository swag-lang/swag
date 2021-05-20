


struct point
{
	x, y: f32
}

//#[Swag.printbc]
#test
{
	var pt: point
	@assert(@sizeof(pt) == 8)
}
