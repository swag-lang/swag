#module module677
//#unittest pass syntax

struct Point
{
	var x: f32
	var y: f32
}

//#[swag.printbc]
#test
{
	var pt : Point(y: 1.0, x: 1.0)
}
