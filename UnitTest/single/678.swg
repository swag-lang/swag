#module module678
//#unittest pass syntax

struct Point
{
	var x: f32
	var y: f32
	var z: f32
}

#unittest error
//#[swag.printbc]
#test
{
	var pt : Point(y: 1.0, x: 1.0, 1.0)
}
