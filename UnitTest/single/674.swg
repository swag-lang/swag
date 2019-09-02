#module module674
//#unittest pass syntax

//#[swag.printbc]
#test
{
	var a : f32
	var b : f64
	c := a + b
	@assert(@op(c, sizeof) == 8)
}
