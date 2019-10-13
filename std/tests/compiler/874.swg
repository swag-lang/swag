#module module874

//#[swag.printbc]
#test
{
	var square = func(x: f32)->f32 { return x * x; }
	var v = square(2)
	@assert(v == 4)
}