#global testerror "Err0521"

struct Point
{
	x: f32
	y: f32
}

//#[Swag.PrintBc]
#test
{
	var pt : Point{1.0, 1.0, true}
}
