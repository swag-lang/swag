#module module520
#unittest pass semantic

//#[swag.printbc]
struct point
{
	x, y: f32
}

#test
{
	var pt: point
	@assert(@sizeof(pt) == 8)
}
