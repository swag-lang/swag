#module "compiler676"


struct Point
{
	x: f32
	y: f32
}

//#[swag.printbc]
#unittest error
#test
{
	var pt : Point(1.0, 1.0, true)
}
