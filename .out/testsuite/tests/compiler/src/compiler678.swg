#global testerror

struct Point
{
	x: f32
	y: f32
	z: f32
}

//#[Swag.printbc]
#test
{
	var pt : Point(y: 1.0, x: 1.0, 1.0)
}
