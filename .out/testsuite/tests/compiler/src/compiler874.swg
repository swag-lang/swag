

//#[Swag.printbc]
#test
{
	var square = @(x: f32)->f32 { return x * x; }
	var v = square(2)
	@assert(v == 4)
}