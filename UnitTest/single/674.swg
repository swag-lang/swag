#module module674
//#unittest pass syntax

//#[swag.printbc]
#test
{
	var a : f32
	var b : f64
	c := a + b
	@assert(@prop(c, sizeof) == 8)
}
