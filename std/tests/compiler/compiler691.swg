#module "compiler691"
#unittest error

struct(T) Point
{
	x: T = 5
	y: T = 10
}

alias Point2f = Point'f32

//#[swag.printbc]
#test
{
	var pt : Point2f(1.0, 1)
}