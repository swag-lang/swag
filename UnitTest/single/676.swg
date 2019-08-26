#module module676
//#unittest pass syntax

struct Point
{
	var x: f32
	var y: f32
}

//#[swag.printbc]
#unittest error
#test
{
	var pt : Point(1.0, 1.0, true)
}
