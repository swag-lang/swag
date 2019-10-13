#module module520
#unittest pass semantic

struct point
{
	x, y: f32
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(@sizeof(pt) == 8)
}
