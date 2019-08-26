#module module675
//#unittest pass syntax

struct Point
{
	var x: f32
	var y: f32
}

//#[swag.printbc]
#test
{
	var pt : Point(1.0, 1.0)
}
