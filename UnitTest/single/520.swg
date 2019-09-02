#module module520
#unittest pass semantic

//#[swag.printbc]
struct point
{
	var x, y: f32
}

#test
{
	var pt: point
	@assert(@op(pt, sizeof) == 8)
}
