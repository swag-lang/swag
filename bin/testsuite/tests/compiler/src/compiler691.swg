struct(T) Point
{
	x: T = 5
	y: T = 10
}

alias Point2f = Point'f32

//#[Swag.PrintBc]
#test
{
	var pt : Point2f{1.0, 1}
}